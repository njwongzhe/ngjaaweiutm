import java.util.ArrayList;
import java.util.Objects;

public class Order {
    private int orderID;
    private Customer customer;      // Association which is a first step toward aggregation.
    private String address;
    private String dateTime;        // Automatically set to local time once it is instantiated.
    private String paymentMethod;
    private double discount;
    private String promoCode;
    private ArrayList<OrderItem> items  = new ArrayList<OrderItem>();;

    private static int idGenerator = 0;

    public Order() {
        this(new Customer());
    }

    public Order(Customer customer) {
        this.orderID = ++idGenerator;
        this.customer = customer;
        this.address = "Unknown";
        this.dateTime = DateTimeRecorder.getDateTime();
        this.paymentMethod = "Unpaid";
    }

    public String toCSVString() {
        StringBuilder sb = new StringBuilder();
        sb.append(orderID).append(",");
        sb.append(escapeComma(customer.getName())).append(",");
        sb.append(escapeComma(customer.getPhone())).append(",");
        sb.append(escapeComma(address)).append(",");
        sb.append(escapeComma(paymentMethod)).append(",");
        sb.append(escapeComma(dateTime)).append(",");
        sb.append(discount).append(",");
        sb.append(escapeComma(promoCode != null ? promoCode : "")).append(",");

        for (OrderItem item : items) {
            MenuItem mi = item.getMenuItem();
            if (mi instanceof Combo) {
                Combo combo = (Combo) mi;
                sb.append("COMBO:").append(combo.getMenuItemID()).append(":")
                  .append(item.getQuantity()).append(":")
                  .append(escapeComma(item.getRemarks())).append(":");
                
                // Escape colons in description lines and join with ||.
                ArrayList<String> escapedDescLines = new ArrayList<>();
                for (String line : combo.getDescriptionLines()) {
                    escapedDescLines.add(escapeColon(line));
                }
                sb.append(escapeComma(String.join("||", escapedDescLines))).append(":")
                  .append(combo.getPrice()).append(":");
                
                // Save exchange information if available.
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
            sb.append(";"); // Use semicolon between items.
        }
        return sb.toString();
    }

    public static Order fromCSVString(String line, ArrayList<MenuItem> allMenuItems) {
        String[] parts = line.split(",", -1);
        if (parts.length < 8) return null;

        Order order = new Order();
        order.orderID = Integer.parseInt(parts[0]);
        order.customer = new Customer(unescapeComma(parts[1]), unescapeComma(parts[2]));
        order.address = unescapeComma(parts[3]);
        order.paymentMethod = unescapeComma(parts[4]);
        order.dateTime = unescapeComma(parts[5]);
        order.discount = Double.parseDouble(parts[6]);
        order.promoCode = parts[7].isEmpty() ? null : unescapeComma(parts[7]);

        if (parts.length >= 9 && !parts[8].isEmpty()) {
            String[] itemEntries = parts[8].split(";");
            for (String entry : itemEntries) {
                if (entry.isEmpty()) continue;

                // Split by colon, but handle escaped colons in description.
                ArrayList<String> details = splitByColon(entry);
                if (details.size() < 3) continue;

                String itemType = details.get(0);
                String itemID = details.get(1);
                int quantity = Integer.parseInt(details.get(2));
                String remarks = unescapeComma(details.get(3));

                MenuItem menuItem = null;

                if (itemType.equals("COMBO")) {
                    // Handle combo items.
                    if (details.size() >= 6) {
                        String description = unescapeComma(details.get(4));
                        double price = Double.parseDouble(details.get(5));

                        // Create temporary combo with saved details.
                        Combo tempCombo = new Combo(itemID);
                        MenuItem originalItem = findItemById(allMenuItems, itemID);
                        tempCombo.setName(originalItem != null ? originalItem.getName() : "Unknown Combo");
                        tempCombo.setPrice(price);

                        // Split and unescape description lines.
                        if (!description.isEmpty()) {
                            for (String ln : description.split("\\|\\|")) {
                                tempCombo.getDescriptionLines().add(unescapeColon(ln));
                            }
                        }

                        // Add exchange information if available.
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
                    // Handle regular items.
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
        result.add(current.toString()); // Add the last segment.
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
        // First, separate paid and unpaid orders.
        ArrayList<Order> paidOrders = new ArrayList<>();
        ArrayList<Order> unpaidOrders = new ArrayList<>();
        
        for (Order order : orders) {
            if (order.getPaymentMethod().equalsIgnoreCase("unpaid")) {
                unpaidOrders.add(order);
            } else {
                paidOrders.add(order);
            }
        }

        // Sort paid orders in ascending order according to dateTime.
        for (int i = 0; i < paidOrders.size(); i++) {
            for (int j = i + 1; j < paidOrders.size(); j++) {
                if (DateTimeRecorder.compareDateTime(paidOrders.get(i).dateTime, paidOrders.get(j).dateTime) > 0) {
                    Order temp = paidOrders.get(i);
                    paidOrders.set(i, paidOrders.get(j));
                    paidOrders.set(j, temp);
                }
            }
        }

        // Assign IDs to paid orders with date-based reset.
        int currentID = 1;
        String currentDate = "";
        
        for (Order order : paidOrders) {
            String orderDate = order.getDateTime().split(" ")[0];
            
            if (!orderDate.equals(currentDate)) {
                currentDate = orderDate;
                currentID = 1;
            }
            
            order.orderID = currentID++;
        }

        // Now assign IDs to unpaid orders (using sequential IDs after the last paid order).
        int unpaidStartID = currentID;
        for (Order order : unpaidOrders) {
            order.orderID = unpaidStartID++;
        }

        // Update the ID generator to the highest ID used.
        idGenerator = (unpaidStartID - 1);
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

    public String getAddress() {
        return address;
    }

    public void setAddress(String address) {
        this.address = address;
    }

    public double getDiscount() {
        return discount;
    }

    public void setDiscount(double discount) {
        this.discount = discount;
    }

    public String getPromoCode() {
        return promoCode;
    }

    public void setPromoCode(String promoCode) {
        this.promoCode = promoCode;
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

    public double getDiscountedTotal() {
        double total = 0.0;
        for(OrderItem i: items) {
            total += i.getSubTotal();
        }
        return total - discount;
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