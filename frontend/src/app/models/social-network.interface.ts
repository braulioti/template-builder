/**
 * Interface representing a social network link
 */
export interface SocialNetwork {
  /** The name of the social network */
  name: string;
  /** Font Awesome icon class (e.g., 'fab fa-twitter') */
  faClass: string;
  /** URL to the social network profile */
  url: string;
}
