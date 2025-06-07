package src;

import java.util.Scanner;

public class Food extends MenuItem {
    protected String category;
    protected String description;

    public Food() {
        super("F0"); //temporary ID for loadingFile(fromCSVString)
        this.category = "Unknown";
        this.description = "";
    }

    public Food(String menuItemID) {
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
        return String.format("Food,%s,%.2f,%s,%s,%s",
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

    public static Food fromCSVString(String[] parts) {
        if (parts.length < 6) return null;

        Food food = new Food();
        food.name = unescapeComma(parts[1]);
        food.price = Double.parseDouble(parts[2]);
        food.menuItemID = parts[3];
        food.category = unescapeComma(parts[4]);
        food.description = unescapeComma(parts[5]);
        return food;
    }

    public static Food readFoodDetails(Scanner read) {
        Food food = new Food(generateNewID("Food"));
        
        // Reuse common input handling from superclass
        readCommonDetails(food, read);
        
        // Read category
        System.out.println("Select category:");
        food.setCategory(CategoryManager.selectFoodCategory(read));

        // Food-specific details
        System.out.print("Enter food description: ");
        food.setDescription(read.nextLine().trim());
        
        return food;
    }
}
