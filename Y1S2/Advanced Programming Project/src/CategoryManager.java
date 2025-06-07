package src;

import java.util.ArrayList;
import java.util.Scanner;
import java.io.*;

public class CategoryManager {
    private static ArrayList<String> foodCategories = new ArrayList<String>();
    private static ArrayList<String> drinkCategories = new ArrayList<String>();
    private static final String FOOD_CATEGORIES_FILE = "food_categories.csv";
    private static final String DRINK_CATEGORIES_FILE = "drink_categories.csv";

    static {
        loadCategories();
    }

    private static void loadCategories() {
        // Load food categories
        try {
            BufferedReader reader = new BufferedReader(new FileReader(FOOD_CATEGORIES_FILE));
            String line;
            while ((line = reader.readLine()) != null) {
                foodCategories.add(line.trim());
            }
            reader.close();
        } catch (IOException e) {
            // Initialize with default food categories if file doesn't exist
            foodCategories.add("Burgers");
            foodCategories.add("Fried Chicken");
            foodCategories.add("Rice Meal");
            foodCategories.add("Snack");
            foodCategories.add("Dessert");
            saveFoodCategories();
        }

        // Load drink categories
        try {
            BufferedReader reader = new BufferedReader(new FileReader(DRINK_CATEGORIES_FILE));
            String line;
            while ((line = reader.readLine()) != null) {
                drinkCategories.add(line.trim());
            }
            reader.close();
        } catch (IOException e) {
            // Initialize with default drink categories if file doesn't exist
            drinkCategories.add("Hot Drinks");
            drinkCategories.add("Soft Drinks");
            drinkCategories.add("Smoothies");
            drinkCategories.add("Milkshakes");
            drinkCategories.add("Juices");
            saveDrinkCategories();
        }
    }

    private static void saveFoodCategories() {
        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(FOOD_CATEGORIES_FILE));
            for (String category : foodCategories) {
                writer.write(category);
                writer.newLine();
            }
            writer.close();
        } catch (IOException e) {
            System.out.println("Error saving food categories: " + e.getMessage());
        }
    }

    private static void saveDrinkCategories() {
        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(DRINK_CATEGORIES_FILE));
            for (String category : drinkCategories) {
                writer.write(category);
                writer.newLine();
            }
            writer.close();
        } catch (IOException e) {
            System.out.println("Error saving drink categories: " + e.getMessage());
        }
    }

    public static ArrayList<String> getFoodCategories() {
        return new ArrayList<String>(foodCategories);
    }

    public static ArrayList<String> getDrinkCategories() {
        return new ArrayList<String>(drinkCategories);
    }

    public static void printFoodCategories() {
        System.out.println("\nFood Categories:");
        for (int i = 0; i < foodCategories.size(); i++) {
            System.out.printf("%d. %s\n", i + 1, foodCategories.get(i));
        }
    }

    public static void printDrinkCategories() {
        System.out.println("\nDrink Categories:");
        for (int i = 0; i < drinkCategories.size(); i++) {
            System.out.printf("%d. %s\n", i + 1, drinkCategories.get(i));
        }
    }

    public static void printCategories() {
        System.out.println("\n===== Food Categories ======");
        for (int i = 0; i < foodCategories.size(); i++) {
            System.out.printf("%d. %s\n", i + 1, foodCategories.get(i));
        }
        
        System.out.println("\n===== Drink Categories =====");
        for (int i = 0; i < drinkCategories.size(); i++) {
            System.out.printf("%d. %s\n", foodCategories.size() + i + 1, drinkCategories.get(i));
        }

        System.out.println("\n========== Combo ===========");
        System.out.printf("%d. Combo\n", foodCategories.size() + drinkCategories.size() + 1);

        System.out.println("\n============================");
    }

    public static boolean addFoodCategory(Scanner read) {
        System.out.printf("Enter new food category name: ");
        String newCategory = read.nextLine().trim();
        
        if (newCategory.isEmpty()) {
            System.out.println("Category name cannot be empty!");
            return false;
        }
        
        // Check if category already exists (non-case sensitive)
        for (String existing : foodCategories) {
            if (existing.toLowerCase().equals(newCategory.toLowerCase())) {
                System.out.println("Category already exists!");
                return false;
            }
        }
        
        foodCategories.add(newCategory);
        saveFoodCategories();
        System.out.println("Food category added successfully!");
        return true;
    }

    public static boolean addDrinkCategory(Scanner read) {
        System.out.printf("Enter new drink category name: ");
        String newCategory = read.nextLine().trim();
        
        if (newCategory.isEmpty()) {
            System.out.println("Category name cannot be empty!");
            return false;
        }
        
        // Check if category already exists (non-case sensitive)
        for (String existing : drinkCategories) {
            if (existing.toLowerCase().equals(newCategory.toLowerCase())) {
                System.out.println("Category already exists!");
                return false;
            }
        }
        
        drinkCategories.add(newCategory);
        saveDrinkCategories();
        System.out.println("Drink category added successfully!");
        return true;
    }

    public static boolean removeFoodCategory(Scanner read, ArrayList<MenuItem> menuItems) {
        printFoodCategories();
        System.out.printf("Enter category number to remove (0 to cancel): ");
        try {
            int choice = Integer.parseInt(read.nextLine());
            if (choice == 0) return false;
            
            if (choice > 0 && choice <= foodCategories.size()) {
                String toRemove = foodCategories.get(choice - 1);
                
                // Check if any food items use this category
                boolean isUsed = false;
                for (MenuItem item : menuItems) {
                    if (item instanceof Food && ((Food)item).getCategory().toLowerCase().equals(toRemove.toLowerCase())) {
                        isUsed = true;
                        break;
                    }
                }
                
                if (isUsed) {
                    System.out.println("Cannot remove - category is in use by food items!");
                    return false;
                }
                
                foodCategories.remove(choice - 1);
                saveFoodCategories();
                System.out.println("Food category removed successfully!");
                return true;
            } else {
                System.out.println("Invalid category number!");
            }
        } catch (NumberFormatException e) {
            System.out.println("Please enter a valid number!");
        }
        return false;
    }

    public static boolean removeDrinkCategory(Scanner read, ArrayList<MenuItem> menuItems) {
        printDrinkCategories();
        System.out.printf("Enter category number to remove (0 to cancel): ");
        try {
            int choice = Integer.parseInt(read.nextLine());
            if (choice == 0) return false;
            
            if (choice > 0 && choice <= drinkCategories.size()) {
                String toRemove = drinkCategories.get(choice - 1);
                
                // Check if any drink items use this category
                boolean isUsed = false;
                for (MenuItem item : menuItems) {
                    if (item instanceof Drink && ((Food)item).getCategory().toLowerCase().equals(toRemove.toLowerCase())) {
                        isUsed = true;
                        break;
                    }
                }
                
                if (isUsed) {
                    System.out.println("Cannot remove - category is in use by drink items!");
                    return false;
                }
                
                drinkCategories.remove(choice - 1);
                saveDrinkCategories();
                System.out.println("Drink category removed successfully!");
                return true;
            } else {
                System.out.println("Invalid category number!");
            }
        } catch (NumberFormatException e) {
            System.out.println("Please enter a valid number!");
        }
        return false;
    }

    public static String selectFoodCategory(Scanner read) {
        while (true) {
            printFoodCategories();
            System.out.printf("Select food category number (or 0 to enter custom category): ");
            String input = read.nextLine();
            
            try {
                int choice = Integer.parseInt(input);
                if (choice == 0) {
                    System.out.printf("Enter custom food category name: ");
                    String customCategory = read.nextLine().trim();
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
                            saveFoodCategories();
                        }
                        return customCategory;
                    }
                } else if (choice > 0 && choice <= foodCategories.size()) {
                    return foodCategories.get(choice - 1);
                } else {
                    System.out.println("Invalid choice!");
                }
            } catch (NumberFormatException e) {
                // Treat as custom category
                if (!input.isEmpty()) {
                    // Check if already exists (nonn-case sensitive)
                    boolean exists = false;
                    for (String cat : foodCategories) {
                        if (cat.toLowerCase().equals(input.toLowerCase())) {
                            exists = true;
                            break;
                        }
                    }
                    
                    if (!exists) {
                        foodCategories.add(input);
                        saveFoodCategories();
                    }
                    return input;
                }
            }
        }
    }

    public static String selectDrinkCategory(Scanner read) {
        while (true) {
            printDrinkCategories();
            System.out.printf("Select drink category number (or 0 to enter custom category): ");
            String input = read.nextLine();
            
            try {
                int choice = Integer.parseInt(input);
                if (choice == 0) {
                    System.out.printf("Enter custom drink category name: ");
                    String customCategory = read.nextLine().trim();
                    if (!customCategory.isEmpty()) {
                        // Check if already exists (non-case sensitive)
                        boolean exists = false;
                        for (String cat : drinkCategories) {
                            if (cat.toLowerCase().equals(customCategory.toLowerCase())) {
                                exists = true;
                                break;
                            }
                        }
                        
                        if (!exists) {
                            drinkCategories.add(customCategory);
                            saveDrinkCategories();
                        }
                        return customCategory;
                    }
                } else if (choice > 0 && choice <= drinkCategories.size()) {
                    return drinkCategories.get(choice - 1);
                } else {
                    System.out.println("Invalid choice!");
                }
            } catch (NumberFormatException e) {
                // Treat as custom category
                if (!input.isEmpty()) {
                    // Check if already exists (non-case sensitive)
                    boolean exists = false;
                    for (String cat : drinkCategories) {
                        if (cat.toLowerCase().equals(input.toLowerCase())) {
                            exists = true;
                            break;
                        }
                    }
                    
                    if (!exists) {
                        drinkCategories.add(input);
                        saveDrinkCategories();
                    }
                    return input;
                }
            }
        }
    }

    public static void manageCategories(Scanner read) {
        while (true) {
            System.out.println("\n===== Manage Categories =====");
            System.out.println("0. Back to main menu");
            System.out.println("1. Manage Food Categories");
            System.out.println("2. Manage Drink Categories");
            System.out.printf("Choose an option: ");
            
            String choice = read.nextLine();
            
            switch (choice) {
                case "0":
                    return;
                case "1":
                    manageFoodCategories(read);
                    break;
                case "2":
                    manageDrinkCategories(read);
                    break;
                default:
                    System.out.println("Invalid option. Please try again.");
            }
        }
    }

    private static void manageFoodCategories(Scanner read) {
        while (true) {
            System.out.println("\n===== Manage Food Categories =====");
            System.out.println("0. Back");
            System.out.println("1. View all food categories");
            System.out.println("2. Add new food category");
            System.out.println("3. Remove food category");
            System.out.printf("Choose an option: ");
            
            String choice = read.nextLine();
            
            switch (choice) {
                case "0":
                    return;
                case "1":
                    printFoodCategories();
                    break;
                case "2":
                    addFoodCategory(read);
                    break;
                case "3":
                    // Need to pass menu items to check if category is in use
                    removeFoodCategory(read, MenuItem.getAllMenuItems());
                    break;
                default:
                    System.out.println("Invalid option. Please try again.");
            }
        }
    }

    private static void manageDrinkCategories(Scanner read) {
        while (true) {
            System.out.println("\n===== Manage Drink Categories =====");
            System.out.println("0. Back");
            System.out.println("1. View all drink categories");
            System.out.println("2. Add new drink category");
            System.out.println("3. Remove drink category");
            System.out.printf("Choose an option: ");
            
            String choice = read.nextLine();
            
            switch (choice) {
                case "0":
                    return;
                case "1":
                    printDrinkCategories();
                    break;
                case "2":
                    addDrinkCategory(read);
                    break;
                case "3":
                    // Need to pass menu items to check if category is in use
                    removeDrinkCategory(read, MenuItem.getAllMenuItems());
                    break;
                default:
                    System.out.println("Invalid option. Please try again.");
            }
        }
    }
}