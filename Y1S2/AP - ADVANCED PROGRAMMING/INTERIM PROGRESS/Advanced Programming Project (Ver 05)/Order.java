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
                sb.append("COMBO:").append(mi.getName()).append(":");
            } else {
                sb.append("ITEM:").append(mi.getName()).append(":");
            }
            sb.append(item.getQuantity()).append(":")
            .append(escapeComma(item.getRemarks())).append("|");
        }
        return sb.toString();
    }

    public static Order fromCSVString(String line, ArrayList<MenuItem> menuItems) {
        String[] parts = line.split(",", -1);
        if (parts.length < 5) return null;

        Order order = new Order();
        order.orderID = Integer.parseInt(parts[0]);
        order.customer = new Customer(unescapeComma(parts[1]), unescapeComma(parts[2]));
        order.paymentMethod = unescapeComma(parts[3]);
        order.dateTime = unescapeComma(parts[4]);

        if (parts.length >= 6) {
            String[] itemParts = parts[5].split("\\|");
            for (String itemStr : itemParts) {
                if (itemStr.isEmpty()) continue;
                
                String[] detail = itemStr.split(":", -1);
                if (detail.length < 4) continue;

                String itemType = detail[0]; // "COMBO" or "ITEM"
                String itemName = unescapeComma(detail[1]);
                int qty = Integer.parseInt(detail[2]);
                String remarks = unescapeComma(detail[3]);

                MenuItem found = null;
                for (MenuItem mi : menuItems) {
                    if (mi.getName().equals(itemName) && 
                        ((itemType.equals("COMBO") && mi instanceof Combo) ||
                        (itemType.equals("ITEM") && !(mi instanceof Combo)))) {
                        found = mi;
                        break;
                    }
                }
                
                if (found != null) {
                    order.addItem(new OrderItem(found, qty, remarks));
                }
            }
        }
        return order;
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
