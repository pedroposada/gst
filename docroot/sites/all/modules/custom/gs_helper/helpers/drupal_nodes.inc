<?php
/**
 * @file
 * Contains helper code for Nodes.
 */

/**
 * Sorts node object alphabetically by title.
 * @return 
 * @param object $a
 * @param object $b
 */
function alphabetize_nodes($a, $b) {
  return strcmp($a->title, $b->title);
}

/**
 * Sorts objects by weight property.
 * @return 
 * @param object $a
 * @param object $b
 */
function order_by_weight($a, $b) {
  $a = $a->weight;
  $b = $b->weight;
  
  if ($a == $b) {
    return 0;
  }

  return ($a < $b) ? -1 : 1;
}

