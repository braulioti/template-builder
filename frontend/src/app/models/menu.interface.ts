/**
 * Interface representing a menu item
 */
export interface Menu {
  /** The route path for the menu item */
  route: string;
  /** The description/label of the menu item */
  description: string;
  /** Optional array of child menu items (for submenus) */
  childrens?: Menu[];
}
