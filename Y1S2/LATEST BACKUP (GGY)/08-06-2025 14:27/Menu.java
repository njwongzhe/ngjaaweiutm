import java.util.ArrayList;
import java.util.Scanner;

public class Menu {
    protected ArrayList<MenuItem> items = new ArrayList<MenuItem>();

    Menu() {
        this.items.addAll(FileManager.loadFoods("foods.csv"));
        this.items.addAll(FileManager.loadDrinks("drinks.csv"));
        this.items.addAll(FileManager.loadCombos("combos.csv", this.items));
        FileManager.initializeMenuIDs();
    }

    public void printMenuItems() {
        System.out.println();
        System.out.println("==================== MENU ====================");
        for(MenuItem item : items) {
            System.out.println(item.getMenuItemID() + ". " + item.getName() + String.format(" - RM%.2f", item.getPrice()));
            item.printDescription();
            System.out.println();
        }
        System.out.println("==============================================");
        System.out.println();
    }

    public MenuItem findItemByName(String name) {
        for(MenuItem item : items) {
            if(item.getName().toLowerCase().equals(name.toLowerCase())) {
                return item;
            }
        }
        return null;
    }

    public void showItemByName(String name) {
        boolean result = false;
        
        System.out.println("\n==============================================");
        for(MenuItem item : items) {
            if(item.getName().toLowerCase().startsWith(name.toLowerCase())) {
                System.out.println(item.getMenuItemID() + ". " + item.getName() + String.format(" - RM%.2f", item.getPrice()));
                item.printDescription();
                System.out.println();
                result = true;
            }
        }

        if(!result) {
            System.out.println("No items found that start with \"" + name + "\".");
        }
        System.out.println("==============================================\n");
    }

    public void getItemByCategory(String input) {
        ArrayList<String> allCategories = new ArrayList<>();
        allCategories.addAll(CategoryManager.getFoodCategories());
        allCategories.addAll(CategoryManager.getDrinkCategories());
        allCategories.add("Combo");
        
        String category = null;
        
        // Check if input is a number
        try {
            int choice = Integer.parseInt(input);
            if (choice > 0 && choice <= allCategories.size()) {
                category = allCategories.get(choice - 1);
            } else {
                System.out.println("Invalid category number!");
                System.out.println();
                return;
            }
        } catch (NumberFormatException e) {
            // Not a number, treat as category name
            category = input;
        }

        boolean found = false;
        System.out.println("\nItems in category: " + category);
        System.out.println("============================");
        
        for (MenuItem item : items) {
            String itemCategory = null;
            
            if (item instanceof Food) {
                itemCategory = ((Food)item).getCategory();
            } else if (item instanceof Drink) {
                itemCategory = ((Drink)item).getCategory();
            } else if (item instanceof Combo) {
                itemCategory = "Combo";
            }
            
            if (itemCategory != null && itemCategory.toLowerCase().equals(category.toLowerCase())) {
                System.out.println(item.getMenuItemID() + ". " + item.getName() + 
                    String.format(" - RM%.2f", item.getPrice()));
                item.printDescription();
                System.out.println();
                found = true;
            }
        }

        if (!found) {
            System.out.println("No items found in category \"" + category + "\".\n");
        }
        System.out.println("============================");
    }

    public static Menu readAccessType(Scanner read) {
        System.out.println("--- Welcome to Group 8 Fast Food Order Record Management System ---");
        System.out.println("                      Make Your Day With Food!                     ");
        System.out.println("Please select your access type.");
        System.out.println("1. Customer.");
        System.out.println("2. Admin.");
        
        int choice = 0;
        do {
            System.out.printf("Enter your choice (1 - 2): ");
            try {
                choice = Integer.parseInt(read.nextLine());
                if (choice != 1 && choice != 2) {
                System.out.println("\nOnly inputs either 1 or 2 are accepted. Please try again.\n");
                }
            } catch(NumberFormatException e) {
                System.out.println("\nInvalid input. Please try again.");
            }

        } while(choice != 1 && choice != 2);

        if(choice == 1 ){
            Customer customer = Customer.readCustomerInfo(read); 
            return new CustomerMenu(customer);
        } else {
            return AdminMenu.loginOrSignup(read);
        }
    }

    public void printMenuOption() {
        System.out.println("0. Log out.");
        System.out.println("1. Show Menu.");
        System.out.println("2. Search Item.");
        System.out.println("3. Filter by Category.");
    }

    public boolean readMenuOption(Scanner read) {
        int choice = -1;
        do {
            this.printMenuOption();

            if (this instanceof AdminMenu) {
                System.out.printf("Enter your choice (0 - 9): ");
                try {
                    choice = Integer.parseInt(read.nextLine());
                    if (choice < 0 || choice > 9) {
                    System.out.println("Only inputs (0 - 9) are accepted. Please try again.");
                    continue;
                    }
                } catch(NumberFormatException e) {
                    System.out.println("Invalid input. Please try again.");
                    continue;
                }
                switch (choice) {
                    case 0:
                        System.out.println("\nExiting...\n");
                        break;
                    case 1: // View menu
                        this.printMenuItems();
                        break;
                    case 2:
                        System.out.printf("\nEnter the item name you want to find: ");
                        String findingName = read.nextLine();
                        this.showItemByName(findingName);
                        break;
                    case 3:
                        CategoryManager.printCategories();
                        System.out.printf("Enter the item category number or name you want to find: ");
                        String category = read.nextLine();
                        this.getItemByCategory(category);
                        break;
                    case 4: 
                        ((AdminMenu)this).readAddItem(read);  
                        break;
                    case 5: 
                        System.out.printf("Enter item name to remove: ");
                        String removeName = read.nextLine();
                        ((AdminMenu) this).removeItem(removeName);
                        break;
                    case 6: 
                        ((AdminMenu)this).findMostPopularItems();
                        break;
                    case 7:
                        ((AdminMenu)this).filterOrdersByDate(read);
                        break;
                    case 8:
                        ((AdminMenu)this).generateDailySalesReport(read);
                        break;
                    case 9:
                        ((AdminMenu)this).manageCategories(read);
                        break;
                    default:
                        System.out.println("Invalid choice.");
                }
            } else if (this instanceof CustomerMenu) {
                System.out.printf("Enter your choice (0 - 7): ");
                try {
                    choice = Integer.parseInt(read.nextLine());
                    if (choice < 0 || choice > 7) {
                    System.out.println("Only inputs (0 - 7) are accepted. Please try again.");
                    continue;
                    }
                } catch(NumberFormatException e) {
                    System.out.println("Invalid input. Please try again.");
                    continue;
                }
                switch (choice) {
                    case 0:
                        System.out.println("\nExiting...\n");
                        break;
                    case 1: 
                        this.printMenuItems();
                        break;
                    case 2:
                        System.out.printf("Enter the item name you want to find: ");
                        String findingName = read.nextLine();
                        this.showItemByName(findingName);
                        break;
                    case 3:
                        CategoryManager.printCategories();
                        System.out.printf("Enter the item category number or name you want to find: ");
                        String category = read.nextLine();
                        this.getItemByCategory(category);
                        break;
                    case 4: 
                        ((CustomerMenu) this).viewOrder();
                        break;
                    case 5:
                        ((CustomerMenu) this).addOrderItem(read);
                        break;
                    case 6:
                        ((CustomerMenu) this).viewOrder();  // Show current cart items
                        System.out.printf("Enter the item name to remove from the cart: ");
                        String removeName = read.nextLine();

                        boolean removed = ((CustomerMenu) this).removeFromOrder(removeName);

                        if (removed) {
                            System.out.println("Item removed from the order cart.");
                        } else {
                            System.out.println("Item not found in the order cart.");
                        }
                        break;
                    case 7: 
                        ((CustomerMenu) this).makePayment(read);
                        break;
                    default:
                        System.out.println("Invalid choice.");
                }
            }
        } while (choice != 0);
        return true;
    }
}
