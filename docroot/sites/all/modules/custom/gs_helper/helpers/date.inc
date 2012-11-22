<?php
/**
 * @file
 * Contains helper code for Dates.
 */

/**
 * Returns formatted date string from UC English Date format.
 * @return 
 * @param object $string
 */
function yuba_date_string($string) {
  $ts = strtotime($string);
  return yuba_date_ts($ts);
}

/**
 * Returns formatted date from Unix timestamp.
 * @return 
 * @param object $ts
 */
function yuba_date_ts($ts) {
  return date("F j, Y g:ia", $ts);
}