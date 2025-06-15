import java.util.Scanner;

public class Food extends MenuItem {
    protected String category;
    protected String description;
    protected int quantity;

    public Food() {
        super("F0"); //temporary ID for loadingFile(fromCSVString)
        this.category = "Unknown";
        this.description = "";
        this.quantity = 0;
    }

    public Food(String menuItemID) {
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
        return String.format("Food,%s,%.2f,%s,%s,%s,%d", 
                escapeComma(name),
                price,
                menuItemID,
                escapeComma(category),
                escapeComma(description),
                quantity);
    }

    @Override
    public void printDescription() {
        System.out.println("Quantity    : " + quantity);
        System.out.println("Category    : " + category);
        System.out.println("Description : " + description);
    }

    public static Food fromCSVString(String[] parts) {
        if (parts.length < 7) return null;

        Food food = new Food();
        food.name = unescapeComma(parts[1]);
        food.price = Double.parseDouble(parts[2]);
        food.menuItemID = parts[3];
        food.category = unescapeComma(parts[4]);
        food.description = unescapeComma(parts[5]);
        food.quantity = Integer.parseInt(parts[6]);
        return food;
    }

    public static Food readFoodDetails(Scanner read) {
        Food food = new Food(generateNewID("Food"));
        
        // Reuse common input handling from superclass
        readCommonDetails(food, read);

        // Read quantity
        System.out.printf(ColourManager.ouColour() + "Enter Quantity Included: " + ColourManager.reColour()); // Ask Input
        food.setQuantity(readValidNumber(read, 1, Integer.MAX_VALUE));
        
        // Read category
        food.setCategory(CategoryManager.selectFoodCategory(read));

        // Food-specific details
        System.out.println();
        System.out.printf(ColourManager.ouColour() + "Enter Food Description (Press ENTER for Default): " + ColourManager.reColour()); // Ask Input
        System.out.print(ColourManager.inColour());
        String description = read.nextLine().trim(); // Input
        System.out.print(ColourManager.reColour());
        if (description.isEmpty()) {
            // Set default description
            description = food.getQuantity() + " x " + food.getName();
        }
        food.setDescription(description);
        
        return food;
    }
}
