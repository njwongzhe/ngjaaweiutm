import java.util.Scanner;

public class Drink extends MenuItem {
    protected String category;
    protected String description;

    public Drink() {
        super("D0"); //temporary ID for loadingFile(fromCSVString)
        this.category = "Unknown";
        this.description = "";
    }

    public Drink(String menuItemID) {
        super(menuItemID);
        this.category = "Unknown";
        this.description = "";
        allMenuItems.add(this);
    }

    public void setCategory(String category) {
        this.category = category;
    }

    public String getCategory() {
        return this.category;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    @Override
    public String toCSVString() {
        return String.format("Drink,%s,%.2f,%s,%s,%s",
                escapeComma(name),
                price,
                menuItemID,
                escapeComma(category),
                escapeComma(description));
    }

    @Override
    public void printDescription() {
        System.out.println("Category: " + category);
        System.out.println("Description: " + description);
    }

    public static Drink fromCSVString(String[] parts) {
        if (parts.length < 6) return null;

        Drink drink = new Drink();
        drink.name = unescapeComma(parts[1]);
        drink.price = Double.parseDouble(parts[2]);
        drink.menuItemID = parts[3];
        drink.category = unescapeComma(parts[4]);
        drink.description = unescapeComma(parts[5]);
        return drink;
    }

    public static Drink readDrinkDetails(Scanner read) {
        Drink drink = new Drink(generateNewID("Drink"));
        
        // Reuse common input handling from superclass
        readCommonDetails(drink, read);
        
        // Read category
        System.out.println("Select category:");
        drink.setCategory(CategoryManager.selectDrinkCategory(read));

        // Drink-specific details
        System.out.print("Enter drink description: ");
        drink.setDescription(read.nextLine().trim());
        
        return drink;
    }
}
