package src;

public class SalesItem {
    String name;
    int quantity;
    double total;
    String itemType;

    public SalesItem(String name, int quantity, double total, String itemType) {
        this.name = name;
        this.quantity = quantity;
        this.total = total;
        this.itemType = itemType;
    }
}