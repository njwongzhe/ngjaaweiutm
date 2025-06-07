import java.io.*;
import java.util.ArrayList;

public class FileManager {
    public static void saveAdmins(ArrayList<Admin> admins, String filename) {
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(filename))) {
            for (Admin admin : admins) {
                bw.write(admin.getName() + "," + admin.getPassword());
                bw.newLine();
            }
        } catch (IOException e) {
            System.out.println("Error saving admins: " + e.getMessage());
        }
    }

    public static ArrayList<Admin> loadAdmins(String filename) {
        ArrayList<Admin> admins = new ArrayList<Admin>();
        try (BufferedReader br = new BufferedReader(new FileReader(filename))) {
            String line;
            while ((line = br.readLine()) != null) {
                String[] parts = line.split(",", -1);
                if (parts.length == 2) {
                    admins.add(new Admin(parts[0], parts[1]));
                }
            }
        } catch (IOException e) {
            System.out.println("Error loading admins: " + e.getMessage());
        }
        return admins;
    }

    public static void saveCustomers(ArrayList<Customer> customers, String filename) {
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(filename))) {
            for (Customer customer : customers) {
                bw.write(customer.toCSVString());
                bw.newLine();
            }
        } catch (IOException e) {
            System.out.println("Error saving customers: " + e.getMessage());
        }
    }

    public static ArrayList<Customer> loadCustomers(String filename) {
        ArrayList<Customer> customers = new ArrayList<Customer>();
        try (BufferedReader br = new BufferedReader(new FileReader(filename))) {
            String line;
            while ((line = br.readLine()) != null) {
                Customer c = Customer.fromCSVString(line);
                if (c != null) customers.add(c);
            }
        } catch (IOException e) {
            System.out.println("Error loading customers: " + e.getMessage());
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
        Order.initializeIDGenerator(orders);
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(filename))) {
            for (Order order : orders) {
                bw.write(order.toCSVString());
                bw.newLine();
            }
        } catch (IOException e) {
            System.out.println("Error saving orders: " + e.getMessage());
        }
    }

    public static ArrayList<Order> loadOrders(String filename, ArrayList<MenuItem> menuItems) {
        ArrayList<Order> orders = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader(filename))) {
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
            System.out.println("Error loading orders: " + e.getMessage());
        }
        return orders;
    }

    public static void saveFoods(ArrayList<Food> foods, String filename) {
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(filename))) {
            for (Food food : foods) {
                bw.write(food.toCSVString());
                bw.newLine();
            }
        } catch (IOException e) {
            System.out.println("Error saving foods: " + e.getMessage());
        }
    }

    public static ArrayList<Food> loadFoods(String filename) {
        ArrayList<Food> foods = new ArrayList<Food>();
        try (BufferedReader br = new BufferedReader(new FileReader(filename))) {
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
            System.out.println("Error loading foods: " + e.getMessage());
        }
        return foods;
    }

    public static void saveDrinks(ArrayList<Drink> drinks, String filename) {
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(filename))) {
            for (Drink drink : drinks) {
                bw.write(drink.toCSVString());
                bw.newLine();
            }
        } catch (IOException e) {
            System.out.println("Error saving drinks: " + e.getMessage());
        }
    }

    public static ArrayList<Drink> loadDrinks(String filename) {
        ArrayList<Drink> drinks = new ArrayList<Drink>();
        try (BufferedReader br = new BufferedReader(new FileReader(filename))) {
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
            System.out.println("Error loading drinks: " + e.getMessage());
        }
        return drinks;
    }

    public static void saveCombos(ArrayList<Combo> combos, String filename) {
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(filename))) {
            for (Combo combo : combos) {
                bw.write(combo.toCSVString());
                bw.newLine();
            }
        } catch (IOException e) {
            System.out.println("Error saving combos: " + e.getMessage());
        }
    }

    public static ArrayList<Combo> loadCombos(String filename, ArrayList<MenuItem> allItems) {
        ArrayList<Combo> combos = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader(filename))) {
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
            System.out.println("Error loading combos: " + e.getMessage());
        }
        return combos;
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
