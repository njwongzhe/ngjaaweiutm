import java.util.Objects;

public class OrderItem {
    private MenuItem menuItem;
    private int quantity;
    private String sideOrders;   //Extra sauces, add round egg, no lettuce and etc.

    public OrderItem(MenuItem menuItem, int quantity, String sideOrders) {
        this.menuItem = menuItem;
        this.quantity = quantity;
        this.sideOrders = sideOrders;
    }

    public void setMenuItem(MenuItem menuItem) {
        this.menuItem = menuItem;
    }

    public MenuItem getMenuItem() {
        return this.menuItem;
    }

    public void setQuantity(int quantity) {
        this.quantity = quantity;
    }

    public int getQuantity() {
        return this.quantity;
    }

    public void setSideOrders(String sideOrders) {
        this.sideOrders = sideOrders;
    }

    public String getSideOrders() {
        return this.sideOrders;
    }

    public double getSubTotal() {
        return this.menuItem.getPrice() * this.quantity;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        OrderItem other = (OrderItem) obj;
        return this.menuItem.equals(other.menuItem)
            && this.quantity == other.quantity
            && this.sideOrders.toLowerCase().equals(other.sideOrders.toLowerCase());
    }

    @Override
    public int hashCode() {
        return Objects.hash(menuItem, quantity, sideOrders.toLowerCase());
    }
}
