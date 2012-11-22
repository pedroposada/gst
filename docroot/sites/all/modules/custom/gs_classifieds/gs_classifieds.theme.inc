<?php
/**
 * @file
 * Contains code specific to theming functionality.
 */


/**
 * Classifieds section topics tray block
 */
function theme_gs_classifieds_topics_tray($data, $active) {
  $options = $classes = array();
	
  $output = '<ul id="topics-tray">';
  foreach ($data as $topic) {
    $output .= '<li id="tag-' . $topic->tid . '">' . l( $topic->name, 'taxonomy/term/' . $topic->tid, ($topic->tid == $active ? array('attributes' => array('class' => array('active'))) : array()) ) . '</li>';
  }
  $output .= '</ul>';
  $output .= l(t('more'), $_GET['q'], array('attributes' => array('class' => array('more','collapsed')), 'fragment' => 'topics-tray'));
  
  return '<div id="topics-tray-wrapper">'.$output.'</div>';
}

/**
 * Classifieds tabs
 */
function theme_gs_classifieds_tabs($css_classifieds, $css_adult_classifieds) {
  $output = '<ul class="quicktabs-tabs quicktabs-style-nostyle">';
  $output .= '<li class="' . $css_classifieds . 'classifieds first"><a href="/classifieds">CLASSIFIEDS</a></li>';
  $output .= '<li class="' . $css_adult_classifieds . 'adult-classifieds last"><a href="/classifieds/adult">ADULT CLASSIFIEDS <span>18+</span></a></li>';
  $output .= '</ul>';
  $output .= '</div>';
  $output .= '<div class="place-a-classified">';
  $output .= '<a href="https://georgiastraight.onlineads.advpubtech.com/">PLACE A CLASSIFIED +</a>';

  return '<div id="classifieds-tabs-wrapper">'.$output.'</div>';
}

/**
 * Coupons print page
 */

function theme_gs_classifieds_print_coupons($node) {
  global $base_url;
  $term = taxonomy_term_load($node->field_coupon_types['und'][0]['tid']);
  $imagePath = "/" . drupal_get_path('module', 'gs_classifieds') . "/images/";
  
  print '<div class="print-logo"><img class="print-logo" src="' . $base_url . $imagePath . 'logo-print.gif" alt="Straight.com" id="logo" /></div>';
  print '<hr class="print-hr" />';
  print '<h1 class="print-title">' . $node->title . '</h1>';
  print '<div id="article_breadcrumbs"><a href="' . $base_url . '/coupons">Coupons</a> <span class="print-footnote">[1]</span> &raquo; ' . l($term->name, 'taxonomy/term/' . $node->field_coupon_types['und'][0]['tid']) . '</div>';
  print '<h1 class="print-title">' . $node->title . '</h1>';
  print '<div class="date-line"> ' . t('Expires') . ': <span class="date-line">' . $node->field_expiry_date['und'][0]['value'] . '</span></div>';
  print '<hr class="print-hr" />';
  print '<div class="print-source_url"><strong>Source URL:</strong> <a href="/node/' . $node->nid . '">http://www.straight.com/node/' . $node->nid . '</a></div>';
  print '<div class="print-links"><p><strong>Links:</strong><br />[1] ' . $base_url . '/coupons<br />
    [2] ' . $base_url . '/coupons/' . strtolower(str_replace(" ", "-", $term->name)) . '<br />
    [3] http://twitter.com/share<br />';

  print '<form>
    <input type="button" value="Print This Page" onClick="window.print()" />
    </form>';
}
 