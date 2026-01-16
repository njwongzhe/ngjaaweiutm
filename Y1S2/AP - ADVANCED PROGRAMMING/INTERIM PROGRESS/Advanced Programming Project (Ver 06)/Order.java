import java.util.ArrayList;
import java.util.Objects;

public class Order {
    private int orderID;
    private Customer customer;      //Association which is a first step toward aggregation.
    private String dateTime;        //Automatically set to local time once it is instantiated.
    private String paymentMethod;
    private ArrayList<OrderItem> items  = new ArrayList<OrderItem>();;

    private static int idGenerator = 0;

    public Order() {
        this(new Customer());
    }

    public Order(Customer customer) {
        this.orderID = ++idGenerator;
        this.customer = customer;
        this.dateTime = DateTimeRecorder.getDateTime();
        this.paymentMethod = "Unpaid";
    }

    public String toCSVString() {
        StringBuilder sb = new StringBuilder();
        sb.append(orderID).append(",");
        sb.append(escapeComma(customer.getName())).append(",");
        sb.append(escapeComma(customer.getPhone())).append(",");
        sb.append(escapeComma(paymentMethod)).append(",");
        sb.append(escapeComma(dateTime)).append(",");

        for (OrderItem item : items) {
            MenuItem mi = item.getMenuItem();
            if (mi instanceof Combo) {
                Combo combo = (Combo) mi;
                sb.append("COMBO:").append(combo.getMenuItemID()).append(":")
                  .append(item.getQuantity()).append(":")
                  .append(escapeComma(item.getRemarks())).append(":");
                
                // Escape colons in description lines and join with ||
                ArrayList<String> escapedDescLines = new ArrayList<>();
                for (String line : combo.getDescriptionLines()) {
                    escapedDescLines.add(escapeColon(line));
                }
                sb.append(escapeComma(String.join("||", escapedDescLines))).append(":")
                  .append(combo.getPrice()).append(":");
                
                // Save exchange information if available
                if (combo.getExchangeList() != null && !combo.getExchangeList().isEmpty()) {
                    for (int i = 0; i < combo.getExchangeList().size(); i++) {
                        if (i > 0) sb.append("|");
                        ArrayList<MenuItem> exchItems = combo.getExchangeList().get(i);
                        ArrayList<Double> exchFees = combo.getExchangeFees().get(i);
                        for (int j = 0; j < exchItems.size(); j++) {
                            if (j > 0) sb.append(",");
                            sb.append(exchItems.get(j).getMenuItemID())
                              .append("-")
                              .append(exchFees.get(j));
                        }
                    }
                }
            } else {
                sb.append("ITEM:").append(mi.getMenuItemID()).append(":")
                  .append(item.getQuantity()).append(":")
                  .append(escapeComma(item.getRemarks()));
            }
            sb.append(";"); // Use semicolon between items
        }
        return sb.toString();
    }

    public static Order fromCSVString(String line, ArrayList<MenuItem> allMenuItems) {
        String[] parts = line.split(",", -1);
        if (parts.length < 5) return null;

        Order order = new Order();
        order.orderID = Integer.parseInt(parts[0]);
        order.customer = new Customer(unescapeComma(parts[1]), unescapeComma(parts[2]));
        order.paymentMethod = unescapeComma(parts[3]);
        order.dateTime = unescapeComma(parts[4]);

        if (parts.length >= 6 && !parts[5].isEmpty()) {
            String[] itemEntries = parts[5].split(";");
            for (String entry : itemEntries) {
                if (entry.isEmpty()) continue;

                // Split by colon, but handle escaped colons in description
                ArrayList<String> details = splitByColon(entry);
                if (details.size() < 3) continue;

                String itemType = details.get(0);
                String itemID = details.get(1);
                int quantity = Integer.parseInt(details.get(2));
                String remarks = unescapeComma(details.get(3));

                MenuItem menuItem = null;

                if (itemType.equals("COMBO")) {
                    // Handle combo items
                    if (details.size() >= 6) {
                        String description = unescapeComma(details.get(4));
                        double price = Double.parseDouble(details.get(5));

                        // Create temporary combo with saved details
                        Combo tempCombo = new Combo(itemID);
                        MenuItem originalItem = findItemById(allMenuItems, itemID);
                        tempCombo.setName(originalItem != null ? originalItem.getName() : "Unknown Combo");
                        tempCombo.setPrice(price);

                        // Split and unescape description lines
                        if (!description.isEmpty()) {
                            for (String ln : description.split("\\|\\|")) {
                                tempCombo.getDescriptionLines().add(unescapeColon(ln));
                            }
                        }

                        // Add exchange information if available
                        if (details.size() >= 7) {
                            String[] exchangeGroups = details.get(6).split("\\|");
                            for (String group : exchangeGroups) {
                                ArrayList<MenuItem> exchItems = new ArrayList<>();
                                ArrayList<Double> exchFees = new ArrayList<>();

                                String[] exchanges = group.split(",");
                                for (String exchange : exchanges) {
                                    String[] portions = exchange.split("-");
                                    if (portions.length == 2) {
                                        MenuItem exchItem = findItemById(allMenuItems, portions[0]);
                                        if (exchItem != null) {
                                            exchItems.add(exchItem);
                                            exchFees.add(Double.parseDouble(portions[1]));
                                        }
                                    }
                                }

                                tempCombo.getExchangeList().add(exchItems);
                                tempCombo.getExchangeFees().add(exchFees);
                            }
                        }

                        menuItem = tempCombo;
                    }
                } else {
                    // Handle regular items
                    menuItem = findItemById(allMenuItems, itemID);
                }

                if (menuItem != null) {
                    order.addItem(new OrderItem(menuItem, quantity, remarks));
                }
            }
        }
        return order;
    }

    private static ArrayList<String> splitByColon(String input) {
        ArrayList<String> result = new ArrayList<>();
        StringBuilder current = new StringBuilder();
        boolean escaped = false;

        for (char c : input.toCharArray()) {
            if (c == '\\') {
                escaped = true;
                continue;
            }
            if (c == ':' && !escaped) {
                result.add(current.toString());
                current = new StringBuilder();
            } else {
                current.append(c);
            }
            escaped = false;
        }
        result.add(current.toString()); // Add the last segment
        return result;
    }

    private static MenuItem findItemById(ArrayList<MenuItem> items, String id) {
        for (MenuItem item : items) {
            if (item.getMenuItemID().equals(id)) {
                return item;
            }
        }
        return null;
    }

    private static String escapeColon(String val) {
        if (val == null) return "";
        return val.replace(":", "\\:");
    }

    private static String unescapeColon(String val) {
        if (val == null) return "";
        return val.replace("\\:", ":");
    }

    private static String escapeComma(String val) {
        return val.replace(",", "&#44;");
    }

    private static String unescapeComma(String val) {
        return val.replace("&#44;", ",");
    }

    public static void initializeIDGenerator(ArrayList<Order> orders) {
        // Sort the orders in ascending order based on dateTime
        for (int i = 0; i < orders.size(); i++) {
            for (int j = i + 1; j < orders.size(); j++) {
                if (DateTimeRecorder.compareDateTime(orders.get(i).dateTime, orders.get(j).dateTime) > 0) {
                    Order temp = orders.get(i);
                    orders.set(i, orders.get(j));
                    orders.set(j, temp);
                }
            }
        }

        int assignedID = 1;
        // Assign paid order first
        for(Order order : orders) {
            if(!order.paymentMethod.toLowerCase().equals("unpaid")) {
                order.orderID = assignedID;
                assignedID++;
            }
        }

        // Then assign unpaid order
        for(Order order : orders) {
            if(order.paymentMethod.toLowerCase().equals("unpaid")) {
                order.orderID = assignedID;
                assignedID++;
            }
        }

        int maxID = 0;
        for (Order order : orders) {
            if (order.getOrderID() > maxID) {
                maxID = order.getOrderID();
            }
        }
        idGenerator = maxID;
    }

    public void resumeUnpaidOrder() {
        if (this.paymentMethod.toLowerCase().equals("unpaid")) {
            this.dateTime = DateTimeRecorder.getDateTime();
        }
    }

    public static void restartIDGenerator() {
        idGenerator = 0;
    }

    public int getOrderID() {
        return this.orderID;
    }

    public void setCustomer(Customer customer) {
        this.customer = customer;
    }

    public Customer getCustomer() {
        return this.customer;
    }

    public String getDateTime() {
        return this.dateTime;
    }

    public void setPaymentMethod(String paymentMethod) {
        this.paymentMethod = paymentMethod;
    }

    public String getPaymentMethod() {
        return this.paymentMethod;
    }

    public void addItem(OrderItem orderItem) {
        items.add(orderItem);
    }

    public void removeItem(OrderItem orderItem) {
        items.remove(orderItem);
    }

    public ArrayList<OrderItem> getItems() {
        return items;
    }

    public double getTotal() {
        double total = 0.0;
        for(OrderItem i: items) {
            total += i.getSubTotal();
        }
        return total;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Order other = (Order) obj;
        return this.orderID == other.orderID;
    }

    @Override
    public int hashCode() {
        return Objects.hash(orderID);
    }   
}
