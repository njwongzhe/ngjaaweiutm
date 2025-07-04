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

    public void showItemByName(String name) {
        boolean result = false;
        boolean isAdmin = this instanceof AdminMenu;
        
        System.out.println();
        for(MenuItem item : items) {
            if ((isAdmin || item.isAvailable()) && item.getName().toLowerCase().contains(name.toLowerCase())) {
                System.out.println(item.getMenuItemID() + ". " + item.getName() + String.format(" - RM%.2f", item.getPrice()));
                item.printDescription(isAdmin);
                System.out.println();
                result = true;
            }
        }

        if(!result) {
            System.out.println(ColourManager.erColour() + "No items found that start with \"" + name + "\".\n" + ColourManager.reColour()); // Error
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

    public static Menu readAccessType(Scanner read) {
        int choice = 0;
        boolean showMenu = true;
        FileManager.loadColourSettings();

        do {
            if (showMenu) {
                System.out.println(ColourManager.ouColour() + "\n--- Welcome to Group 8 Fast Food Order Record Management System ---"); // Output
                System.out.println(                       "                      Make Your Day With Food!                     ");
                System.out.println(ColourManager.ouColour() + "<Who Are You?>"); // Output
                System.out.println(ColourManager.opColour() + "1. Staff"); // Option
                System.out.println(                           "2. Admin");
                System.out.println(ColourManager.ouColour() + "<Setting>"); // Output
                System.out.println(ColourManager.opColour() + "3. CLI Colour Customization" + ColourManager.reColour()); // Option
            }

            System.out.printf(ColourManager.ouColour() + "Select an Option (1 - 3): " + ColourManager.reColour());

            try {
                System.out.print(ColourManager.inColour()); 
                choice = Integer.parseInt(read.nextLine().trim());
                System.out.print(ColourManager.reColour());

                if (choice == 3) {
                    ColourManager.customizeColour(read);
                    showMenu = true; 
                } else if (choice < 1 || choice > 3) {
                    System.out.println(ColourManager.erColour() + "Only inputs 1 to 3 are accepted. Please try again.\n" + ColourManager.reColour()); // Error
                    showMenu = false;
                } else {
                    break;
                }
            } catch (NumberFormatException e) {
                System.out.println(ColourManager.erColour() + "Invalid input. Please try again.\n" + ColourManager.reColour()); // Error
                showMenu = false;
            }
        } while (true);

        switch (choice) {
            case 1:
                System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                return StaffMenu.loginOrSignup(read);
            case 2:
                System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                return AdminMenu.loginOrSignup(read);
            // Not yet open for Customer Self-Order.
            /*
            case 3:
                Customer customer = Customer.readCustomerInfo(read); 
                return new CustomerMenu(customer);
            */
            default:
                return null;
        }
    }

    public void printMenuOption() {
        System.out.print(ColourManager.opColour());
        System.out.println("0. Log Out"); // Option
        System.out.println("1. Show Menu");
        System.out.println("2. Search Item");
        System.out.println("3. Filter Item by Category");
        System.out.print(ColourManager.reColour());
    }

    public boolean readMenuOption(Scanner read) {
        int choice = -1;
        do {
            this.printMenuOption();

            if (this instanceof AdminMenu) {
                System.out.printf(ColourManager.ouColour() + "Select an Option (0 - 8): " + ColourManager.reColour()); // Ask Input
                try {
                    System.out.print(ColourManager.inColour());
                    choice = Integer.parseInt(read.nextLine()); // Input
                    System.out.print(ColourManager.reColour());
                    if (choice < 0 || choice > 8) {
                    System.out.println(ColourManager.erColour() + "Only inputs (0 - 8) are accepted. Please try again." + ColourManager.reColour()); // Error
                    System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                    continue;
                    }
                } catch(NumberFormatException e) {
                    System.out.println(ColourManager.erColour() + "Invalid input. Please try again." + ColourManager.reColour()); // Error
                    System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                    continue;
                }
                switch (choice) {
                    case 0:
                        System.out.println(ColourManager.suColour() + "\nExiting..." + ColourManager.reColour()); // Success
                        System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                        break;
                    case 1: // View menu.
                        this.printMenuItems();
                        break;
                    case 2:
                        System.out.println(ColourManager.ouColour() + "\n================ Search Item =================\n" + ColourManager.reColour()); // Output
                        System.out.printf(ColourManager.ouColour() + "Enter the Item Name You Want to Find: " + ColourManager.reColour()); // Ask Input
                        System.out.print(ColourManager.inColour());
                        String findingName = read.nextLine(); // Input
                        System.out.print(ColourManager.reColour());
                        this.showItemByName(findingName);
                        System.out.println(ColourManager.ouColour() + "==============================================" + ColourManager.reColour()); // Output
                        break;
                    case 3:
                        System.out.println(ColourManager.ouColour() + "\n================ Item Filter =================\n" + ColourManager.reColour()); // Output
                        CategoryManager.printCategories();
                        System.out.printf(ColourManager.ouColour() + "Enter the Item Category Number or Name You Want to Find: " + ColourManager.reColour()); // Ask Input
                        System.out.print(ColourManager.inColour());
                        String category = read.nextLine(); // Input
                        System.out.print(ColourManager.reColour());
                        this.getItemByCategory(category);
                        break;
                    case 4: 
                        ((AdminMenu)this).manageMenuItems(read);
                        break;
                    case 5:
                        ((AdminMenu)this).manageCategories(read);
                        System.out.println(ColourManager.ouColour() + "==============================================" + ColourManager.reColour()); // Output
                        break;
                    case 6:
                        System.out.println(ColourManager.ouColour() + "\n================ Order Filter ================\n" + ColourManager.reColour()); // Output
                        ((AdminMenu)this).filterOrdersByDate(read);
                        break;
                    case 7: 
                        ((AdminMenu)this).findMostPopularItems();
                        System.out.println(ColourManager.ouColour() + "==============================================" + ColourManager.reColour()); // Output
                        break;
                    case 8:
                        ((AdminMenu)this).generateDailySalesReport(read);
                        System.out.println(ColourManager.ouColour() + "======================================================================" + ColourManager.reColour()); // Output
                        break;
                    default:
                        System.out.println(ColourManager.erColour() + "Invalid choice." + ColourManager.reColour()); // Error
                        System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                }
            } else if (this instanceof CustomerMenu) {
                System.out.printf(ColourManager.ouColour() + "Select an Option (0 - 7): " + ColourManager.reColour()); // Ask Input
                try {
                    System.out.print(ColourManager.inColour());
                    choice = Integer.parseInt(read.nextLine()); // Input
                    System.out.print(ColourManager.reColour());
                    if (choice < 0 || choice > 7) {
                    System.out.println(ColourManager.erColour() + "Only inputs (0 - 7) are accepted. Please try again." + ColourManager.reColour()); // Error
                    System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                    continue;
                    }
                } catch(NumberFormatException e) {
                    System.out.println(ColourManager.erColour() + "Invalid input. Please try again." + ColourManager.reColour()); // Error
                    System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                    continue;
                }
                switch (choice) {
                    case 0:
                        System.out.println(ColourManager.suColour() + "\nExiting..." + ColourManager.reColour()); // Success
                        break;
                    case 1: 
                        this.printMenuItems();
                        break;
                    case 2:
                        System.out.println(ColourManager.ouColour() + "\n================ Search Item =================\n" + ColourManager.reColour()); // Output
                        System.out.printf(ColourManager.ouColour() + "Enter the Item Name You Want to Find: " + ColourManager.reColour()); // Ask Input
                        System.out.print(ColourManager.inColour());
                        String findingName = read.nextLine(); // Input
                        System.out.print(ColourManager.reColour());
                        this.showItemByName(findingName);
                        System.out.println(ColourManager.ouColour() + "==============================================" + ColourManager.reColour()); // Output
                        break;
                    case 3:
                        System.out.println(ColourManager.ouColour() + "\n================ Item Filter =================\n" + ColourManager.reColour()); // Output
                        CategoryManager.printCategories();
                        System.out.printf(ColourManager.ouColour() + "Enter the Item Category Number or Name You Want to Find: " + ColourManager.reColour()); // Ask Input
                        System.out.print(ColourManager.inColour());
                        String category = read.nextLine(); // Input
                        System.out.print(ColourManager.reColour());
                        this.getItemByCategory(category);
                        break;
                    case 4:
                        System.out.println(ColourManager.ouColour() + "\n===================================== View Order =====================================\n" + ColourManager.reColour()); // Output
                        ((CustomerMenu)this).viewOrder();
                        System.out.println(ColourManager.ouColour() + "======================================================================================" + ColourManager.reColour()); // Output
                        break;
                    case 5:
                        System.out.println(ColourManager.ouColour() + "\n============= Add Item to Order ==============\n" + ColourManager.reColour()); // Output
                        ((CustomerMenu)this).addOrderItem(read);
                        System.out.println(ColourManager.ouColour() + "==============================================" + ColourManager.reColour()); // Output
                        System.out.print(ColourManager.reColour());
                        break;
                    case 6:
                        System.out.println(ColourManager.ouColour() + "\n==================================== Remove Order ====================================\n" + ColourManager.reColour()); // Output
                        ((CustomerMenu)this).viewOrder();  // Show current cart items
                        boolean removed = ((CustomerMenu) this).removeFromOrder(read);
                        if (removed) {
                            System.out.println(ColourManager.suColour() + "Item removed successfully.\n" + ColourManager.reColour()); // Success
                        }
                        System.out.println(ColourManager.ouColour() + "======================================================================================" + ColourManager.reColour()); // Output
                        break;
                    case 7: 
                        ((CustomerMenu)this).makePayment(read);
                        break;
                    default:
                        System.out.println(ColourManager.erColour() + "Invalid choice." + ColourManager.reColour()); // Error
                        System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                }
            }
        } while (choice != 0);
        return true;
    }
}
