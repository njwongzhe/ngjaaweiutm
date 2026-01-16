import java.util.ArrayList;
import java.util.Scanner;

public abstract class Menu {
    protected ArrayList<MenuItem> items = new ArrayList<MenuItem>();

    Menu() {
        this.items.addAll(FileManager.loadFoods("foods.csv"));
        this.items.addAll(FileManager.loadDrinks("drinks.csv"));
        this.items.addAll(FileManager.loadCombos("combos.csv", this.items));
        FileManager.initializeMenuIDs();
    }

    public void printMenuItems() {
        System.out.println();
        System.out.println(ColourManager.ouColour() + "==================== MENU ====================" + ColourManager.reColour()); // Output
        System.out.println();
        boolean isAdmin = this instanceof AdminMenu;
        
        for(MenuItem item : items) {
            // For customer menu, skip unavailable items.
            if (!isAdmin && !item.isAvailable()) {
                continue;
            }
            
            System.out.println(item.getMenuItemID() + ". " + item.getName() + String.format(" - RM%.2f", item.getPrice()));
            item.printDescription(isAdmin);  // Pass isAdmin flag.
            System.out.println();
        }
        System.out.println(ColourManager.ouColour() + "==============================================" + ColourManager.reColour()); // Output
    }

    public void showItemByName(String input) {
        boolean result = false;
        boolean isAdmin = this instanceof AdminMenu;
        
        System.out.println();

        for(MenuItem item : items) {
            if ((isAdmin || item.isAvailable()) && item.getMenuItemID().toLowerCase().equals(input.toLowerCase())) {
                System.out.println(item.getMenuItemID() + ". " + item.getName() + String.format(" - RM%.2f", item.getPrice()));
                item.printDescription(isAdmin);
                System.out.println();
                return;
            }
        }

        for(MenuItem item : items) {
            if ((isAdmin || item.isAvailable()) && item.getName().toLowerCase().contains(input.toLowerCase())) {
                System.out.println(item.getMenuItemID() + ". " + item.getName() + String.format(" - RM%.2f", item.getPrice()));
                item.printDescription(isAdmin);
                System.out.println();
                return;
            }
        }

        if(!result) {
            System.out.println(ColourManager.erColour() + "No items found that start with \"" + input + "\".\n" + ColourManager.reColour()); // Error
        }
    }

    public void getItemByCategory(String input) {
        ArrayList<String> allCategories = new ArrayList<>();
        allCategories.addAll(CategoryManager.getFoodCategories());
        allCategories.addAll(CategoryManager.getDrinkCategories());
        allCategories.add("Combo");
        
        String category = null;
        
        // Check if input is a number.
        try {
            int choice = Integer.parseInt(input);
            if (choice > 0 && choice <= allCategories.size()) {
                category = allCategories.get(choice - 1);
            } else {
                System.out.println(ColourManager.erColour() + "Invalid category number!" + ColourManager.reColour()); // Error
                System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                return;
            }
        } catch (NumberFormatException e) {
            // Not a number, treat as category name.
            category = input;
        }

        boolean isAdmin = this instanceof AdminMenu;
        boolean found = false;
        System.out.println(ColourManager.ouColour() + "\n==============================================\n" + ColourManager.reColour()); // Output
        System.out.println("Items in Category: " + category);
        System.out.println();
        
        for (MenuItem item : items) {
            String itemCategory = null;
            
            if (item instanceof Food) {
                itemCategory = ((Food)item).getCategory();
            } else if (item instanceof Drink) {
                itemCategory = ((Drink)item).getCategory();
            } else if (item instanceof Combo) {
                itemCategory = "Combo";
            }
            
            if ((isAdmin || item.isAvailable()) && itemCategory != null && itemCategory.toLowerCase().equals(category.toLowerCase())) {
                System.out.println(item.getMenuItemID() + ". " + item.getName() + 
                    String.format(" - RM%.2f", item.getPrice()));
                item.printDescription(isAdmin);
                System.out.println();
                found = true;
            }
        }

        if (!found) {
            System.out.println(ColourManager.erColour() + "No items found in category \"" + category + "\".\n" + ColourManager.reColour()); // Error
        }
        System.out.println(ColourManager.ouColour() + "==============================================" + ColourManager.reColour()); // Output
    }

    public abstract void printMenuOption();
    public abstract boolean readMenuOption(Scanner read);
}