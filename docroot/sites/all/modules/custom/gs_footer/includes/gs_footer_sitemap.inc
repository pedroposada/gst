<?php

/*
 * @file
 *
 * Adminitration of Sitemap
 * 
 */

/**
 * Generate sitemap
 */
function gs_footer_sitemap_generate() {

  $output = '<h2>Straight.com Site Map</h2>';
  
  $output .= '<div id="sitemap-first-column">';
  $output .= '<ul id="taxonomy-terms">' . l('Homepage', "", array('attributes' => array('class' => array('taxonomy-0')))) . '<span id="top-level-term-sitemap">&raquo;</span></ul>'; // Homepage
  $output .= '<ul id="taxonomy-terms">' . gs_footer_print_term_and_children(610, 'Blog', 5, 0) . "</ul>"; // Blogs
  $output .= '<ul id="taxonomy-terms">' . gs_footer_print_term_and_children(433, 'News', 2, 0) . "</ul>"; // News
  $output .= '<ul id="taxonomy-terms">' . gs_footer_print_term_and_children(430, 'Music', 2, 0) . "</ul>"; // Music
  $output .= '<ul id="taxonomy-terms">' . gs_footer_print_term_and_children(429, 'Arts', 2, 0) . "</ul>"; // Arts
  $output .= '</div><div id="sitemap-second-column">';
  $output .= '<ul id="taxonomy-terms">' . gs_footer_print_term_and_children(439, 'Movies', 2, 0) . "</ul>"; // Movies
  $output .= '<ul id="taxonomy-terms">' . gs_footer_print_term_and_children(437, 'Food', 2, 0) . "</ul>"; // Food
  $output .= '<ul id="taxonomy-terms">' . gs_footer_print_term_and_children(449, 'Life', 2, 0) . "</ul>"; // Life
  
  $listings = array();
  $listings[] = array('name' => 'Movie Listings', 'path' => 'listings');
  $listings[] = array('name' => 'Music Listings', 'path' => 'listings');
  $listings[] = array('name' => 'Dining Listings', 'path' => 'listings');
  $listings[] = array('name' => 'Arts Listings', 'path' => 'listings');
  $listings[] = array('name' => 'Clubs Listings', 'path' => 'listings');
  $listings[] = array('name' => 'Events Listings', 'path' => 'listings');
  $output .= gs_footer_sitemap_get_links(array('name' => 'Listings', 'path' => 'listings'), $listings); // Listings
  $output .= '</div><div id="sitemap-third-column">';
  $output .= '<ul id="taxonomy-terms">' . l('Contests', "contests", array('attributes' => array('class' => array('taxonomy-0')))) . '<span id="top-level-term-sitemap">&raquo;</span></ul>'; // Contests
  $output .= '<ul id="taxonomy-terms">' . l('Store', "store", array('attributes' => array('class' => array('taxonomy-0')))) . '<span id="top-level-term-sitemap">&raquo;</span></ul>'; // Store
  
  $classifieds = array();
  $classifieds[] = array('name' => 'Coupons', 'path' => 'classifieds/coupons');
  $classifieds[] = array('name' => 'Place an Ad', 'path' => 'https://georgiastraight.onlineads.advpubtech.com/');
  $classifieds[] = array('name' => 'Adult Classifieds', 'path' => 'classifieds/adults');
  $classifieds[] = array('name' => 'I Saw You', 'path' => 'isawyou');
  $classifieds[] = array('name' => 'Confessions', 'path' => 'confessions');
  $output .= gs_footer_sitemap_get_links(array('name' => 'Classifieds', 'path' => 'classifieds'), $classifieds); // Classifieds
  
  $corporate = array();
  $corporate[] = array('name' => 'About Us', 'path' => 'about-us');
  $corporate[] = array('name' => 'History', 'path' => 'history');
  $corporate[] = array('name' => 'Contact Us', 'path' => 'contact-us');
  $corporate[] = array('name' => 'Masthead', 'path' => 'masthead');
  $corporate[] = array('name' => 'Careers', 'path' => 'careers');
  $corporate[] = array('name' => 'Back Issues', 'path' => 'backissues');
  $corporate[] = array('name' => 'Feedback', 'path' => 'feedback');
  $corporate[] = array('name' => 'Advertise with Us', 'path' => 'advertise-at-the-straight');
  $corporate[] = array('name' => 'Straight Facts', 'path' => 'straightfacts');
  $corporate[] = array('name' => 'Terms & Conditions', 'path' => 'terms-and-conditions');
  $corporate[] = array('name' => 'Privacy Policy', 'path' => 'privacy-policy');
  $output .= gs_footer_sitemap_get_links(array('name' => 'Corporate', 'path' => NULL), $corporate); // Corporate
  
  $connect = array();
  $connect[] = array('name' => 'Facebook', 'path' => 'http://www.facebook.com/georgiastraight');
  $connect[] = array('name' => 'Twitter', 'path' => 'twitter');
  $connect[] = array('name' => 'Google +', 'path' => 'user');
  $connect[] = array('name' => 'Flicker', 'path' => 'about-us');
  $connect[] = array('name' => 'RSS', 'path' => 'straight-com-rss-feeds');
  $connect[] = array('name' => 'Mobile Site', 'path' => 'http://m.straight.com');
  $connect[] = array('name' => 'Newsletters', 'path' => 'newsletters');
  $output .= gs_footer_sitemap_get_links(array('name' => 'Connect With Us', 'path' => NULL), $connect); // Connect with us
  $output .= '</div>';
  
  return $output;
}

/**
 * Recursive function to output taxonomy tree and filter blacklisted terms
 *
 * @return String 
 *   Returns an html string: $output
 */
function gs_footer_print_term_and_children($tid, $name, $vid, $css_level) {
  $output = '';
  if(!in_array($tid, gs_footer_get_black_list_terms())) {
    $children = taxonomy_get_tree($vid, $tid, 1);
	$path = _gs_footer_section_tid_map_search($tid);

	if(!$path) {
	  $path = 'taxonomy/term/' . $tid;
	}
	if ($css_level == 0) {
	  $output .= l($name, $path, array('attributes' => array('class' => array('taxonomy-' . $css_level)))) . '<span id="top-level-term-sitemap">&raquo;</span></li>';
	}
	else {
	  $output .= '<li id="list-item-' . $css_level . '">' . l($name, $path, array('attributes' => array('class' => array('taxonomy-' . $css_level)))) . '</li>';
	}
	foreach($children as $key => $value) {
	  $output .= gs_footer_print_term_and_children($value->tid, $value->name, $vid, $css_level + 1);
	}
  }
  return $output;
}

/**
 * @return array
 *   Returns an array of blacklisted terms
 */
function gs_footer_get_black_list_terms() {
  $vid = 2; $tid = 861;
  $black_list_tree = taxonomy_get_tree($vid, $tid);
  $black_list_terms = array(861, 461);//additional terms to remove
  $white_list_terms = array(445);//terms to add

  foreach($black_list_tree as $key => $value) {
    array_push($black_list_terms, $value->tid);
  }

  foreach($black_list_terms as $key => $value) {
    if(in_array($value, $white_list_terms)) {
      unset($black_list_terms[$key]);
    }
  }

  return $black_list_terms;
}

/**
 * Gets the path of a section by searching the data passed from _gs_helper_section_tid_map() and returns the key.
 *
 * @param $tid
 *     The term id you are searching for
 * @param $only_parent
 *     If true, the parent section is returned
 *
 * @return string
 *    Returns the key value, and false if none is found
 */
function _gs_footer_section_tid_map_search($tid, $only_parent = FALSE) {
  $section_map = _gs_helper_section_tid_map();

  $to_search = array(
    'tid' => array(
      '0' => $tid,
    ),
  );
 
  foreach ($section_map as $key => $value) {
    $exists = TRUE;
    
    foreach ($to_search as $skey => $svalue) {
      $exists = ($exists && isset($section_map[$key][$skey]) && $section_map[$key][$skey] == $svalue);
    }
    if ($exists) {
      if ($only_parent) {
        $key = array_shift(array_values(explode('/', $key)));
      }
      return $key;
    }
  }
 
  return FALSE;
}

/**
 * 
 * @param $parents
 *     An array of the parent sitemap item
 * @param $terms
 *     An array of the child sitemap terms
 *
 * @return string
 *    Returns an html sting to output
 */
 function gs_footer_sitemap_get_links($parents = array(), $terms = array()) {
  if ($parents['path']) {
    $output = '<ul id="taxonomy-terms">' . l($parents['name'], $parents['path'], array('attributes' => array('class' => array('taxonomy-0')))) . '<span id="top-level-term-sitemap">&raquo;</span>';
  } 
  else {
    $output = '<ul id="taxonomy-terms">' . $parents['name'];
  }
  foreach($terms as $key => $value) {
    $output .= '<li>' . l($value['name'], $value['path'], array('attributes' => array('class' => array('taxonomy-1')))) . '</li>';
  }
  $output .= '</ul>';
  
  return $output;
}
  