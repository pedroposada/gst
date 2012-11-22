<?php
/**
 * @file
 * Contains helper code for Arrays.
 */

/**
 * Sorts $data array by value, preserving keys
 */
function yubasort(&$data) {
  $keys = array_keys($data);
  $values = array_values($data);
  
  if (!empty($keys) && !empty($values)) {
    //reverse keys & values
    $data = array_combine($values, $keys);
    
    //sort by key (which is actually the value)
    ksort($data);
    
    //reverse the keys & values again to get original relationship
    $data = array_combine(array_values($data), array_keys($data));
  }
}