public class SalesItem {
    String itemID;
    String name;
    int quantity;
    double total;
    String itemType;

    public SalesItem(String itemID, String name, int quantity, double total, String itemType) {
        this.itemID = itemID;
        this.name = name;
        this.quantity = quantity;
        this.total = total;
        this.itemType = itemType;
    }
}