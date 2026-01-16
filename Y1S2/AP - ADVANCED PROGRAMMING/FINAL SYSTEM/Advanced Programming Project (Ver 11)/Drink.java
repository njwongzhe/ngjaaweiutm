import java.util.Scanner;

public class Drink extends MenuItem {
    protected String category;
    protected String description;
    protected int quantity;

    public Drink() {
        super("D0"); //temporary ID for loadingFile(fromCSVString)
        this.category = "Unknown";
        this.description = "";
        this.quantity = 0;
    }

    public Drink(String menuItemID) {
        super(menuItemID);
        this.category = "Unknown";
        this.description = "";
        this.quantity = 0;
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
    
    public int getQuantity() {
        return this.quantity;
    }

    public void setQuantity(int quantity) {
        this.quantity = quantity;
    }

    @Override
    public String toCSVString() {
        return String.format("Drink,%s,%.2f,%s,%s,%s,%d,%b", 
                escapeComma(name),
                price,
                menuItemID,
                escapeComma(category),
                escapeComma(description),
                quantity,
                available);
    }

    @Override
    public void printDescription(boolean isAdmin) {
        super.printAvailable(isAdmin);
        System.out.println("Quantity    : " + quantity);
        System.out.println("Category    : " + category);
        System.out.println("Description : " + description);
    }

    public static Drink fromCSVString(String[] parts) {
        if (parts.length < 7) return null;

        Drink drink = new Drink();
        drink.name = unescapeComma(parts[1]);
        drink.price = Double.parseDouble(parts[2]);
        drink.menuItemID = parts[3];
        drink.category = unescapeComma(parts[4]);
        drink.description = unescapeComma(parts[5]);
        drink.quantity = Integer.parseInt(parts[6]);
        if (parts.length > 7) {
            drink.available = Boolean.parseBoolean(parts[7]);
        }
        return drink;
    }

    public static Drink readDrinkDetails(Scanner read) {
        Drink drink = new Drink(generateNewID("Drink"));
        
        // Reuse common input handling from superclass.
        readCommonDetails(drink, read);

        // Read quantity.
        System.out.printf(ColourManager.ouColour() + "Enter Quantity Included: " + ColourManager.reColour()); // Ask Input
        drink.setQuantity(readValidNumber(read, 1, Integer.MAX_VALUE));
        
        // Read category.
        System.out.println();
        drink.setCategory(CategoryManager.selectDrinkCategory(read));

        // Drink-specific details.
        System.out.printf(ColourManager.ouColour() + "\nEnter Drink Description (Press ENTER for Default): " + ColourManager.reColour()); // Ask Input
        System.out.print(ColourManager.inColour());
        String description = read.nextLine().trim(); // Input
        System.out.print(ColourManager.reColour());
        if (description.isEmpty()) {
            // Set default description.
            description = drink.getQuantity() + " x " + drink.getName();
        }
        drink.setDescription(description);
        
        return drink;
    }
}