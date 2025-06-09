import java.io.*;
import java.util.ArrayList;

public class FileManager {
    private static final String SYSTEM_DATA_DIR = "System Data/";

    private static void createDataDirectory() {
        File directory = new File(SYSTEM_DATA_DIR);
        if (!directory.exists()) {
            directory.mkdir();
        }
    }

    public static void saveAdmins(ArrayList<Admin> admins, String filename) {
        createDataDirectory();
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(SYSTEM_DATA_DIR + filename))) {
            for (Admin admin : admins) {
                bw.write(admin.getName() + "," + admin.getPassword());
                bw.newLine();
            }
        } catch (IOException e) {
            System.out.println(ColourManager.erColour() + "Error Saving Admins: " + e.getMessage() + ColourManager.reColour()); // Error
        }
    }

    public static ArrayList<Admin> loadAdmins(String filename) {
        createDataDirectory();
        ArrayList<Admin> admins = new ArrayList<Admin>();
        try (BufferedReader br = new BufferedReader(new FileReader(SYSTEM_DATA_DIR + filename))) {
            String line;
            while ((line = br.readLine()) != null) {
                String[] parts = line.split(",", -1);
                if (parts.length == 2) {
                    admins.add(new Admin(parts[0], parts[1]));
                }
            }
        } catch (IOException e) {
            System.out.println(ColourManager.erColour() + "Error Loading Admins: " + e.getMessage() + ColourManager.reColour()); // Error
        }
        return admins;
    }

    public static void saveCustomers(ArrayList<Customer> customers, String filename) {
        createDataDirectory();
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(SYSTEM_DATA_DIR + filename))) {
            for (Customer customer : customers) {
                bw.write(customer.toCSVString());
                bw.newLine();
            }
        } catch (IOException e) {
            System.out.println(ColourManager.erColour() + "Error Saving Customers: " + e.getMessage() + ColourManager.reColour()); // Error
        }
    }

    public static ArrayList<Customer> loadCustomers(String filename) {
        createDataDirectory();
        ArrayList<Customer> customers = new ArrayList<Customer>();
        try (BufferedReader br = new BufferedReader(new FileReader(SYSTEM_DATA_DIR + filename))) {
            String line;
            while ((line = br.readLine()) != null) {
                Customer c = Customer.fromCSVString(line);
                if (c != null) customers.add(c);
            }
        } catch (IOException e) {
            System.out.println(ColourManager.erColour() + "Error loading customers: " + e.getMessage() + ColourManager.reColour()); // Error
        }

        ArrayList<MenuItem> menuItems = new ArrayList<MenuItem>();
        menuItems.addAll(loadFoods("foods.csv"));
        menuItems.addAll(loadDrinks("drinks.csv"));

        ArrayList<Order> orders = FileManager.loadOrders("orders.csv", menuItems);

        for (Order order : orders) {
            Customer orderCustomer = order.getCustomer();
            for (Customer c : customers) {
                if (c.equals(orderCustomer)) {
                    c.addOrder(order);
                    break;
                }
            }
        }

        return customers;
    }

    public static void saveOrders(ArrayList<Order> orders, String filename) {
        createDataDirectory();
        Order.initializeIDGenerator(orders);
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(SYSTEM_DATA_DIR + filename))) {
            for (Order order : orders) {
                bw.write(order.toCSVString());
                bw.newLine();
            }
        } catch (IOException e) {
            System.out.println(ColourManager.erColour() + "Error saving orders: " + e.getMessage() + ColourManager.reColour()); // Error
        }
    }

    public static ArrayList<Order> loadOrders(String filename, ArrayList<MenuItem> menuItems) {
        createDataDirectory();
        ArrayList<Order> orders = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader(SYSTEM_DATA_DIR + filename))) {
            // First load all menu items including combos
            ArrayList<MenuItem> allMenuItems = new ArrayList<>();
            allMenuItems.addAll(loadFoods("foods.csv"));
            allMenuItems.addAll(loadDrinks("drinks.csv"));
            allMenuItems.addAll(loadCombos("combos.csv", allMenuItems));
            
            String line;
            while ((line = br.readLine()) != null) {
                Order order = Order.fromCSVString(line, allMenuItems);
                if (order != null) {
                    order.getCustomer().addOrder(order); // Link back to customer
                    orders.add(order);
                }
            }
            Order.initializeIDGenerator(orders);
        } catch (IOException e) {
            System.out.println(ColourManager.erColour() + "Error loading orders: " + e.getMessage() + ColourManager.reColour()); // Error
        }
        return orders;
    }

    public static void saveFoods(ArrayList<Food> foods, String filename) {
        createDataDirectory();
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(SYSTEM_DATA_DIR + filename))) {
            for (Food food : foods) {
                bw.write(food.toCSVString());
                bw.newLine();
            }
        } catch (IOException e) {
            System.out.println(ColourManager.erColour() + "Error saving foods: " + e.getMessage() + ColourManager.reColour()); // Error
        }
    }

    public static ArrayList<Food> loadFoods(String filename) {
        createDataDirectory();
        ArrayList<Food> foods = new ArrayList<Food>();
        try (BufferedReader br = new BufferedReader(new FileReader(SYSTEM_DATA_DIR + filename))) {
            String line;
            while ((line = br.readLine()) != null) {
                String[] parts = line.split(",", -1);
                Food food = Food.fromCSVString(parts);
                if (food != null) {
                    foods.add(food);
                    MenuItem.getAllMenuItems().add(food); // Add to global list
                }
                // Convert to MenuItem list, sort, then convert back
                ArrayList<MenuItem> sorted = sortMenuItems(new ArrayList<MenuItem>(foods));
                foods.clear();
                for (MenuItem item : sorted) {
                    foods.add((Food)item);
                }
            }
        } catch (IOException e) {
            System.out.println(ColourManager.erColour() + "Error loading foods: " + e.getMessage() + ColourManager.reColour()); // Error
        }
        return foods;
    }

    public static void saveDrinks(ArrayList<Drink> drinks, String filename) {
        createDataDirectory();
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(SYSTEM_DATA_DIR + filename))) {
            for (Drink drink : drinks) {
                bw.write(drink.toCSVString());
                bw.newLine();
            }
        } catch (IOException e) {
            System.out.println(ColourManager.erColour() + "Error saving drinks: " + e.getMessage() + ColourManager.reColour()); // Error
        }
    }

    public static ArrayList<Drink> loadDrinks(String filename) {
        createDataDirectory();
        ArrayList<Drink> drinks = new ArrayList<Drink>();
        try (BufferedReader br = new BufferedReader(new FileReader(SYSTEM_DATA_DIR + filename))) {
            String line;
            while ((line = br.readLine()) != null) {
                String[] parts = line.split(",", -1);
                Drink drink = Drink.fromCSVString(parts);
                if (drink != null) {
                    drinks.add(drink);
                    MenuItem.getAllMenuItems().add(drink); // Add to global list
                }
                // Convert to MenuItem list, sort, then convert back
                ArrayList<MenuItem> sorted = sortMenuItems(new ArrayList<MenuItem>(drinks));
                drinks.clear();
                for (MenuItem item : sorted) {
                    drinks.add((Drink)item);
                }
            }
        } catch (IOException e) {
            System.out.println(ColourManager.erColour() + "Error loading drinks: " + e.getMessage() + ColourManager.reColour()); // Error
        }
        return drinks;
    }

    public static void saveCombos(ArrayList<Combo> combos, String filename) {
        createDataDirectory();
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(SYSTEM_DATA_DIR + filename))) {
            for (Combo combo : combos) {
                bw.write(combo.toCSVString());
                bw.newLine();
            }
        } catch (IOException e) {
            System.out.println(ColourManager.erColour() + "Error saving combos: " + e.getMessage() + ColourManager.reColour()); // Error
        }
    }

    public static ArrayList<Combo> loadCombos(String filename, ArrayList<MenuItem> allItems) {
        createDataDirectory();
        ArrayList<Combo> combos = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader(SYSTEM_DATA_DIR + filename))) {
            String line;
            while ((line = br.readLine()) != null) {
                String[] parts = line.split(",", -1);
                Combo combo = Combo.fromCSVString(parts, allItems);
                if (combo != null) {
                    combos.add(combo);
                    MenuItem.getAllMenuItems().add(combo);
                }
                // Convert to MenuItem list, sort, then convert back
                ArrayList<MenuItem> sorted = sortMenuItems(new ArrayList<MenuItem>(combos));
                combos.clear();
                for (MenuItem item : sorted) {
                    combos.add((Combo)item);
                }
            }
        } catch (IOException e) {
            System.out.println(ColourManager.erColour() + "Error loading combos: " + e.getMessage() + ColourManager.reColour()); // Error
        }
        return combos;
    }

    public static void loadCategories(ArrayList<String> foodCategories, ArrayList<String> drinkCategories) {
        // Load food categories
        try {
            BufferedReader reader = new BufferedReader(new FileReader(SYSTEM_DATA_DIR + "food_categories.csv"));
            String line;
            while ((line = reader.readLine()) != null) {
                foodCategories.add(line.trim());
            }
            reader.close();
        } catch (IOException e) {
            // Initialize with default food categories if file doesn't exist
            foodCategories.add("Burger");
            foodCategories.add("Classic");
            foodCategories.add("Special");
            foodCategories.add("Chicken");
            foodCategories.add("Side Order");
            saveFoodCategories(foodCategories);
        }

        // Load drink categories
        try {
            BufferedReader reader = new BufferedReader(new FileReader(SYSTEM_DATA_DIR + "drink_categories.csv"));
            String line;
            while ((line = reader.readLine()) != null) {
                drinkCategories.add(line.trim());
            }
            reader.close();
        } catch (IOException e) {
            // Initialize with default drink categories if file doesn't exist
            drinkCategories.add("Cold");
            drinkCategories.add("Hot");
            drinkCategories.add("Ice Cream");
            saveDrinkCategories(drinkCategories);
        }
    }

    public static void saveFoodCategories(ArrayList<String> foodCategories) {
        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(SYSTEM_DATA_DIR + "food_categories.csv"));
            for (String category : foodCategories) {
                writer.write(category);
                writer.newLine();
            }
            writer.close();
        } catch (IOException e) {
            System.out.println(ColourManager.erColour() + "Error saving food categories: " + e.getMessage() + ColourManager.reColour()); // Error
        }
    }

    public static void saveDrinkCategories(ArrayList<String> drinkCategories) {
        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(SYSTEM_DATA_DIR + "drink_categories.csv"));
            for (String category : drinkCategories) {
                writer.write(category);
                writer.newLine();
            }
            writer.close();
        } catch (IOException e) {
            System.out.println(ColourManager.erColour() + "Error saving drink categories: " + e.getMessage() + ColourManager.reColour()); // Error
        }
    }

    public static void saveReceipts(ArrayList<String> lines, String filename) {
        createDataDirectory();
        // Create Receipts subdirectory if it doesn't exist
        File receiptsDir = new File(SYSTEM_DATA_DIR + "Receipts");
        if (!receiptsDir.exists()) {
            receiptsDir.mkdir();
        }
        
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(SYSTEM_DATA_DIR + "Receipts/" + filename + ".txt"))) {
            for (String line : lines) {
                bw.write(line);
                bw.newLine();
            }
        } catch (IOException e) {
            System.out.println(ColourManager.erColour() + "Error saving receipt: " + e.getMessage() + ColourManager.reColour()); // Error
        }
    }

    public static void initializeMenuIDs() {
        MenuItem.initializeIDGenerator(MenuItem.getAllMenuItems());
    }

    private static ArrayList<MenuItem> sortMenuItems(ArrayList<MenuItem> items) {
        ArrayList<MenuItem> sortedItems = new ArrayList<MenuItem>();
        
        int maxID = 0;
        
        // First find the maximum ID number
        for (MenuItem item : items) {
            int currentID = Integer.parseInt(item.getMenuItemID().substring(1));
            if (currentID > maxID) {
                maxID = currentID;
            }
        }
        
        // Now add items in order from 1 to maxID
        for (int i = 1; i <= maxID; i++) {
            for (MenuItem item : items) {
                if (Integer.parseInt(item.getMenuItemID().substring(1)) == i) {
                    sortedItems.add(item);
                    break;
                }
            }
        }
        
        return sortedItems;
    }
}