import java.util.ArrayList;
import java.util.Scanner;

public class CategoryManager {
    private static ArrayList<String> foodCategories = new ArrayList<String>();
    private static ArrayList<String> drinkCategories = new ArrayList<String>();

    static {
        FileManager.loadCategories(foodCategories, drinkCategories);
    }

    public static ArrayList<String> getFoodCategories() {
        return new ArrayList<String>(foodCategories);
    }

    public static ArrayList<String> getDrinkCategories() {
        return new ArrayList<String>(drinkCategories);
    }

    public static void printFoodCategories() {
        System.out.println(ColourManager.ouColour() + "<Food Categories>" + ColourManager.reColour()); // Output
        System.out.print(ColourManager.opColour());
        for (int i = 0; i < foodCategories.size(); i++) {
            System.out.printf("%d. %s\n", i + 1, foodCategories.get(i)); // Option
        }
        System.out.print(ColourManager.reColour());
    }

    public static void printDrinkCategories() {
        System.out.println(ColourManager.ouColour() + "<Drink Categories>" + ColourManager.reColour()); // Output
        System.out.print(ColourManager.opColour());
        for (int i = 0; i < drinkCategories.size(); i++) {
            System.out.printf("%d. %s\n", i + 1, drinkCategories.get(i));
        }
        System.out.print(ColourManager.reColour());
    }

    public static void printCategories() {
        System.out.println(ColourManager.ouColour() + "\n<Food Categories>" + ColourManager.reColour()); // Output
        System.out.print(ColourManager.opColour());
        for (int i = 0; i < foodCategories.size(); i++) { // Option
            System.out.printf("%d. %s\n", i + 1, foodCategories.get(i));
        }
        System.out.print(ColourManager.reColour());
        System.out.println();
        
        System.out.println(ColourManager.ouColour() + "\n<Drink Categories>" + ColourManager.reColour()); // Output
        System.out.print(ColourManager.opColour());
        for (int i = 0; i < drinkCategories.size(); i++) { // Option
            System.out.printf("%d. %s\n", foodCategories.size() + i + 1, drinkCategories.get(i)); 
        }
        System.out.print(ColourManager.reColour());
        System.out.println();

        System.out.println(ColourManager.ouColour() + "\n<Combo>" + ColourManager.reColour()); // Output
        System.out.print(ColourManager.opColour());
        System.out.printf("%d. Combo\n", foodCategories.size() + drinkCategories.size() + 1); // Option
        System.out.print(ColourManager.reColour());
        System.out.println();
    }

    public static boolean addFoodCategory(Scanner read) {
        System.out.printf(ColourManager.ouColour() + "Enter New Food Category Name: " + ColourManager.reColour()); // Ask Input
        System.out.print(ColourManager.inColour());
        String newCategory = read.nextLine().trim(); // Input
        System.out.print(ColourManager.reColour());
        
        if (newCategory.isEmpty()) {
            System.out.println(ColourManager.erColour() + "\nCategory name cannot be empty!" + ColourManager.reColour()); // Error
            return false;
        }
        
        // Check if category already exists (non-case sensitive).
        for (String existing : foodCategories) {
            if (existing.toLowerCase().equals(newCategory.toLowerCase())) {
                System.out.println(ColourManager.erColour() + "\nCategory already exists!" + ColourManager.reColour()); // Error
                return false;
            }
        }
        
        foodCategories.add(newCategory);
        FileManager.saveFoodCategories(foodCategories);
        System.out.println(ColourManager.suColour() + "Food category added successfully!" + ColourManager.reColour()); // Success
        return true;
    }

    public static boolean addDrinkCategory(Scanner read) {
        System.out.printf(ColourManager.ouColour() + "Enter New Drink Category Name: " + ColourManager.reColour()); // Ask Input
        System.out.print(ColourManager.inColour());
        String newCategory = read.nextLine().trim(); // Input
        System.out.print(ColourManager.reColour());
        
        if (newCategory.isEmpty()) {
            System.out.println(ColourManager.erColour() + "\nCategory name cannot be empty!" + ColourManager.reColour()); // Error
            return false;
        }
        
        // Check if category already exists (non-case sensitive).
        for (String existing : drinkCategories) {
            if (existing.toLowerCase().equals(newCategory.toLowerCase())) {
                System.out.println(ColourManager.erColour() + "\nCategory already exists!" + ColourManager.reColour()); // Error
                return false;
            }
        }
        
        drinkCategories.add(newCategory);
        FileManager.saveDrinkCategories(drinkCategories);
        System.out.println(ColourManager.suColour() + "Drink category added successfully!" + ColourManager.reColour()); // Success
        return true;
    }

    public static boolean removeFoodCategory(Scanner read, ArrayList<MenuItem> menuItems) {
        printFoodCategories();
        System.out.printf(ColourManager.ouColour() + "Enter Category Number to Remove (0 to Cancel): " + ColourManager.reColour()); // Ask Input
        try {
            System.out.print(ColourManager.inColour());
            int choice = Integer.parseInt(read.nextLine()); // Input
            System.out.print(ColourManager.reColour());
            if (choice == 0) return false;
            
            if (choice > 0 && choice <= foodCategories.size()) {
                String toRemove = foodCategories.get(choice - 1);
                
                // Check if any food items use this category.
                boolean isUsed = false;
                for (MenuItem item : menuItems) {
                    if (item instanceof Food && ((Food)item).getCategory().toLowerCase().equals(toRemove.toLowerCase())) {
                        isUsed = true;
                        break;
                    }
                }
                
                if (isUsed) {
                    System.out.println(ColourManager.erColour() + "Cannot remove - category is in use by food items!" + ColourManager.reColour()); // Error
                    return false;
                }
                
                foodCategories.remove(choice - 1);
                FileManager.saveFoodCategories(foodCategories);
                System.out.println(ColourManager.suColour() + "Food category removed successfully!" + ColourManager.reColour()); // Success
                return true;
            } else {
                System.out.println(ColourManager.erColour() + "Invalid category number!" + ColourManager.reColour()); // Error
            }
        } catch (NumberFormatException e) {
            System.out.println(ColourManager.erColour() + "Please enter a valid number!" + ColourManager.reColour()); // Error
        }
        return false;
    }

    public static boolean removeDrinkCategory(Scanner read, ArrayList<MenuItem> menuItems) {
        printDrinkCategories();
        System.out.printf(ColourManager.ouColour() + "Enter Category Number to Remove (0 to Cancel): " + ColourManager.reColour()); // Ask Input
        try {
            System.out.print(ColourManager.inColour());
            int choice = Integer.parseInt(read.nextLine()); // Input
            System.out.print(ColourManager.reColour());
            if (choice == 0) return false;
            
            if (choice > 0 && choice <= drinkCategories.size()) {
                String toRemove = drinkCategories.get(choice - 1);
                
                // Check if any drink items use this category.
                boolean isUsed = false;
                for (MenuItem item : menuItems) {
                    if (item instanceof Drink && ((Drink)item).getCategory().toLowerCase().equals(toRemove.toLowerCase())) {
                        isUsed = true;
                        break;
                    }
                }
                
                if (isUsed) {
                    System.out.println(ColourManager.erColour() + "Cannot remove - category is in use by drink items!" + ColourManager.reColour()); // Error
                    return false;
                }
                
                drinkCategories.remove(choice - 1);
                FileManager.saveDrinkCategories(drinkCategories);
                System.out.println(ColourManager.suColour() + "Drink category removed successfully!" + ColourManager.reColour()); // Success
                return true;
            } else {
                System.out.println(ColourManager.erColour() + "Invalid category number!" + ColourManager.reColour()); // Error
            }
        } catch (NumberFormatException e) {
            System.out.println(ColourManager.erColour() + "Please enter a valid number!" + ColourManager.reColour()); // Error
        }
        return false;
    }

    public static String selectFoodCategory(Scanner read) {
        while (true) {
            printFoodCategories();
            System.out.printf(ColourManager.ouColour() + "Select Food Category Number (OR 0 to Enter Custom Category): " + ColourManager.reColour()); // Ask Input
            System.out.print(ColourManager.inColour());
            String input = read.nextLine().trim(); // Input
            System.out.print(ColourManager.reColour());

            while (input.isEmpty()) {
                System.out.println(ColourManager.erColour() + "No input detected.\n" + ColourManager.reColour()); // Error
                System.out.printf(ColourManager.ouColour() + "Select Food Category Number (OR 0 to Enter Custom Category): " + ColourManager.reColour()); // Ask Input
                input = read.nextLine().trim();
            }
            
            try {
                int choice = Integer.parseInt(input);
                if (choice == 0) {
                    System.out.printf(ColourManager.ouColour() + "Enter Custom Food Category Name: " + ColourManager.reColour()); // Ask Input
                    System.out.print(ColourManager.inColour());
                    String customCategory = read.nextLine().trim(); // Input
                    System.out.print(ColourManager.reColour());
                    if (!customCategory.isEmpty()) {
                        // Check if already exists (non-case sensitive)
                        boolean exists = false;
                        for (String cat : foodCategories) {
                            if (cat.toLowerCase().equals(customCategory.toLowerCase())) {
                                exists = true;
                                break;
                            }
                        }
                        
                        if (!exists) {
                            foodCategories.add(customCategory);
                            FileManager.saveFoodCategories(foodCategories);
                        }
                        return customCategory;
                    }
                } else if (choice > 0 && choice <= foodCategories.size()) {
                    return foodCategories.get(choice - 1);
                } else {
                    System.out.println(ColourManager.erColour() + "Invalid choice!\n" + ColourManager.reColour()); // Error
                }
            } catch (NumberFormatException e) {
                // Treat as custom category.
                if (!input.isEmpty()) {
                    // Check if already exists (non-case sensitive).
                    boolean exists = false;
                    for (String cat : foodCategories) {
                        if (cat.toLowerCase().equals(input.toLowerCase())) {
                            exists = true;
                            break;
                        }
                    }
                    
                    if (!exists) {
                        foodCategories.add(input);
                        FileManager.saveFoodCategories(foodCategories);
                    }
                    return input;
                }
            }
        }
    }

    public static String selectDrinkCategory(Scanner read) {
        while (true) {
            printDrinkCategories();
            System.out.printf(ColourManager.ouColour() + "Select Drink Category Number (OR 0 to Enter Custom Category): " + ColourManager.reColour()); // Ask Input
            System.out.print(ColourManager.inColour());
            String input = read.nextLine().trim(); // Input
            System.out.print(ColourManager.reColour());

            while (input.isEmpty()) {
                System.out.println(ColourManager.erColour() + "\nNo input detected." + ColourManager.reColour()); // Error
                System.out.printf(ColourManager.ouColour() + "Select Drink Category Number (OR 0 to Enter Custom Category): " + ColourManager.reColour()); // Ask Input
                input = read.nextLine().trim();
            }
            
            try {
                int choice = Integer.parseInt(input);
                if (choice == 0) {
                    System.out.printf(ColourManager.ouColour() + "Enter Custom Drink Category Name: " + ColourManager.reColour()); // Ask Input
                    System.out.print(ColourManager.inColour());
                    String customCategory = read.nextLine().trim(); // Input
                    System.out.print(ColourManager.reColour());
                    if (!customCategory.isEmpty()) {
                        // Check if already exists (non-case sensitive).
                        boolean exists = false;
                        for (String cat : drinkCategories) {
                            if (cat.toLowerCase().equals(customCategory.toLowerCase())) {
                                exists = true;
                                break;
                            }
                        }
                        
                        if (!exists) {
                            drinkCategories.add(customCategory);
                            FileManager.saveDrinkCategories(drinkCategories);
                        }
                        return customCategory;
                    }
                } else if (choice > 0 && choice <= drinkCategories.size()) {
                    return drinkCategories.get(choice - 1);
                } else {
                    System.out.println(ColourManager.erColour() + "Invalid choice!" + ColourManager.reColour()); // Error
                }
            } catch (NumberFormatException e) {
                // Treat as custom category.
                if (!input.isEmpty()) {
                    // Check if already exists (non-case sensitive).
                    boolean exists = false;
                    for (String cat : drinkCategories) {
                        if (cat.toLowerCase().equals(input.toLowerCase())) {
                            exists = true;
                            break;
                        }
                    }
                    
                    if (!exists) {
                        drinkCategories.add(input);
                        FileManager.saveDrinkCategories(drinkCategories);
                    }
                    return input;
                }
            }
        }
    }

    public static void manageCategories(Scanner read) {
        while (true) {
            System.out.println(ColourManager.ouColour() + "\n============= Manage Categories ==============\n" + ColourManager.reColour()); // Output
            System.out.println(ColourManager.ouColour() + "<Manage>" + ColourManager.reColour()); // Output
            System.out.print(ColourManager.opColour());
            System.out.println("0. Back to Main Menu"); // Option
            System.out.println("1. Manage Food Categories");
            System.out.println("2. Manage Drink Categories");
            System.out.print(ColourManager.reColour());
            System.out.printf(ColourManager.ouColour() + "Select an Option (0 - 2): " + ColourManager.reColour()); // Ask Input
            
            System.out.print(ColourManager.inColour());
            String choice = read.nextLine(); // Input
            System.out.print(ColourManager.reColour());
            
            switch (choice) {
                case "0":
                    System.out.println();
                    return;
                case "1":
                    manageFoodCategories(read);
                    break;
                case "2":
                    manageDrinkCategories(read);
                    break;
                default:
                    System.out.println(ColourManager.erColour() + "Invalid option. Please try again." + ColourManager.reColour()); // Error
            }
        }
    }

    private static void manageFoodCategories(Scanner read) {
        while (true) {
            System.out.println(ColourManager.ouColour() + "\n=========== Manage Food Categories ===========\n" + ColourManager.reColour()); // Output
            System.out.println(ColourManager.ouColour() + "<Manage Food>" + ColourManager.reColour()); // Output
            System.out.print(ColourManager.opColour());
            System.out.println("0. Back"); // Option
            System.out.println("1. View All Food Categories");
            System.out.println("2. Add New Food Category");
            System.out.println("3. Remove Food Category");
            System.out.print(ColourManager.reColour());
            System.out.printf(ColourManager.ouColour() + "Select an Option (0 - 3): " + ColourManager.reColour()); // Ask Input
            
            System.out.print(ColourManager.inColour());
            String choice = read.nextLine(); // Input
            System.out.print(ColourManager.reColour());
            
            switch (choice) {
                case "0":
                    return;
                case "1":
                    System.out.println(ColourManager.ouColour() + "\n============== Food Categories ===============\n" + ColourManager.reColour()); // Output
                    printFoodCategories();
                    break;
                case "2":
                    System.out.println(ColourManager.ouColour() + "\n============= Add Food Category ==============\n" + ColourManager.reColour()); // Output
                    addFoodCategory(read);
                    break;
                case "3":
                    // Need to pass menu items to check if category is in use.
                    System.out.println(ColourManager.ouColour() + "\n============ Remove Food Category ============\n" + ColourManager.reColour()); // Output
                    removeFoodCategory(read, MenuItem.getAllMenuItems());
                    break;
                default:
                    System.out.println(ColourManager.erColour() + "Invalid option. Please try again." + ColourManager.reColour()); // Error
            }
        }
    }

    private static void manageDrinkCategories(Scanner read) {
        while (true) {
            System.out.println(ColourManager.ouColour() + "\n========== Manage Drink Categories ===========\n" + ColourManager.reColour()); // Output
            System.out.println(ColourManager.ouColour() + "<Manage Drink>" + ColourManager.reColour()); // Output
            System.out.print(ColourManager.opColour());
            System.out.println("0. Back"); // Option
            System.out.println("1. View All Drink Categories");
            System.out.println("2. Add New Drink Category");
            System.out.println("3. Remove Drink Category");
            System.out.print(ColourManager.reColour());
            System.out.printf(ColourManager.ouColour() + "Select an Option (0 - 3): " + ColourManager.reColour()); // Ask Input
            
            System.out.print(ColourManager.inColour());
            String choice = read.nextLine(); // Input
            System.out.print(ColourManager.reColour());
            
            switch (choice) {
                case "0":
                    return;
                case "1":
                    System.out.println(ColourManager.ouColour() + "\n============== Drink Categories ==============\n" + ColourManager.reColour()); // Output
                    printDrinkCategories();
                    break;
                case "2":
                    System.out.println(ColourManager.ouColour() + "\n============= Add Drink Category =============\n" + ColourManager.reColour()); // Output
                    addDrinkCategory(read);
                    break;
                case "3":
                    // Need to pass menu items to check if category is in use.
                    System.out.println(ColourManager.ouColour() + "\n=========== Remove Drink Category ============\n" + ColourManager.reColour()); // Output
                    removeDrinkCategory(read, MenuItem.getAllMenuItems());
                    break;
                default:
                    System.out.println(ColourManager.erColour() + "Invalid option. Please try again." + ColourManager.reColour()); // Error
            }
        }
    }
}