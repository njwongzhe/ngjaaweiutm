import java.util.Objects;

public class OrderItem {
    private MenuItem menuItem;
    private int quantity;
    private String remarks;   //Extra sauces, add round egg, no lettuce and etc.

    public OrderItem(MenuItem menuItem, int quantity, String remarks) {
        this.menuItem = menuItem;
        this.quantity = quantity;
        this.remarks = remarks;
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

    public void setRemarks(String remarks) {
        this.remarks = remarks;
    }

    public String getRemarks() {
        return this.remarks;
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
            && this.remarks.toLowerCase().equals(other.remarks.toLowerCase());
    }

    @Override
    public int hashCode() {
        return Objects.hash(menuItem, quantity, remarks.toLowerCase());
    }
}
