<?php

/*
 * @file
 *
 * Administration of Classifieds
 * 
 */

function classifieds_admin_menu() {

  //$output = _gsclassifieds_return_breadcrumbs("");

  $admin_links = array();
  
  $admin_links['items'] = array(l("Import Ads from txt file", "admin/workbench/classifieds/import"), l("Operate on Ads", "admin/workbench/classifieds/operations"));
  $admin_links['title'] = 'Classifieds Administration Options';
  $admin_links['type'] = 'ul';
  $admin_links['attributes'] = array('title' => t('Import Ads from txt file'));
  $output = theme_item_list($admin_links);

  $query = "SELECT count(1) AS ad_count FROM {node} WHERE type='content_classified_ad'";

  $result = db_query($query);

  if ($result) {

    foreach ($result as $record) {
      $output .= 'There are currently ' . $record->ad_count . ' ads.';
    }
  }

  return $output;
}


/*BULK*/

function classifieds_import() {

  return drupal_get_form('classifieds_import_getfile');
}

function classifieds_import_getfile() {

  $form = array();

  $form['filename'] = array(
    '#type' => 'file',
    '#title' => t('Classified ad file to upload'),
  );

  $form['#method'] = 'post';

  // Form need encoding type for file upload
  $form['#attributes'] = array("enctype" => "multipart/form-data");
   
  // Create a submit button
  $form['submit'] = array('#type' => 'submit', '#value' => t('Upload File'));

  return $form;   
}


/*=============================================================*/


function classifieds_import_getfile_submit($form, &$form_state) {

   $form_values = $form_state['values'];

  // Submit routines do not directly produce any output
  // They do something with the form values
  // which is typically to store them in the database
  // then return a path to determine what page is shown next

  $file = file_save_upload('filename');
  if ($_POST['op'] == t('Upload')) {
    // already been saved to temporary location
    watchdog('classadimport', t("Uploaded file: %file", array('%file' => theme('placeholder', $file->filename))));
  }

  drupal_set_message(classifieds_form_upload($file));
}

function classifieds_form_upload($file = NULL) {

  include_once(drupal_get_path('module', 'gs_classifieds') . '/includes/gsadmin_classified_form_upload.inc');

  return classified_form_upload_do($file);
}

function _gsclassifieds_clean_body($text) {

  /**
  print "<br>------ Ad Body BEFORE START------<br>";
  print "<code>" . str_replace(">", "&gt;", str_replace("<", "&lt;", $text)) . "</code>";
  print "<br>--------------------------------<br>";
  print $text;
  print "<br>------ Ad Body BEFORE END------<br>";
  **/
  $body = trim($text, '"');
  
  // Replace ZapfDingbats with bullets
  $dingbat = '/(<FONT face="ZapfDingbats">)(<[^>]*>)*(.*?)(<\/[^>]*>)*(<\/FONT>)/';
  $bullet = '${2}•${4}';
  //$body = str_replace($dingbat, $bullet, $body);
  $body = preg_replace($dingbat, $bullet, $body);
  $body = preg_replace("/\x95/","&#8226;",$body); // bullet 
  
  // Replace curly quotes with straight quotes
  $body = preg_replace("/\x93/","&#8220;",$body); // left quote 
  $body = preg_replace("/\x94/","&#8221",$body); // right quote 
  
  // Replace black font with bold tags
  //$blackfont = '/(<FONT face="Helvetica LT Std Cond Blk">)(.+?)(<\/FONT>)/';
  //$boldtags = '<b>${2}</b>';
  //$body = preg_replace($blackfont, $boldtags, $body);
  $blackfont = '/(<FONT face="Helvetica LT Std Cond Blk">)(.*?)(<\/FONT>)/';
  $boldtags = '<b>${2}</b>';
  $body = preg_replace($blackfont, $boldtags, $body);
  $blackfont = '/(<FONT face="Helvetica [\w\s]*">)(.*?)(<\/FONT>)/';
  $boldtags = '${2}';
  $body = preg_replace($blackfont, $boldtags, $body);
  
  $bluefont = '/(<FONT COLOR="Blue">)(.*?)(<\/FONT>)/';
  $bluetags = '<span style="color:blue;">${2}</span>';
  $body = preg_replace($bluefont, $bluetags, $body);
  $blackfont = '/(<FONT COLOR="Black">)(.*?)(<\/FONT>)/';
  $blacktags = '<span style="color:black;">${2}</span>';
  $body = preg_replace($blackfont, $blacktags, $body);
  $redfont = '/(<FONT COLOR="Red">)(.*?)(<\/FONT>)/';
  $redtags = '<span style="color:red;">${2}</span>';
  $body = preg_replace($redfont, $redtags, $body);
  $greenfont = '/(<FONT COLOR="Green">)(.*?)(<\/FONT>)/';
  $greentags = '<span style="color:green;">${2}</span>';
  $body = preg_replace($greenfont, $greentags, $body);
  $magentafont = '/(<FONT COLOR="Magenta">)(.*?)(<\/FONT>)/';
  $magentatags = '<span style="color:magenta;">${2}</span>';
  $body = preg_replace($magentafont, $magentatags, $body);  
  
  // Remove remaining font references
  $fonttags = '/(<FONT.*?>)(.*?)(<\/FONT>)/';
  $enclosedtext = '${2}';
  $body = preg_replace($fonttags, $enclosedtext, $body);
  
  // Remove any font tags that have no close tag (this probably isn't needed if the above is corret regep)
  $unclosedfonttags = '/<FONT.*?>/';
  $nochar = '';
  $body = str_replace($unclosedfonttags, $nochar, $body);
  
  // Remove orphaned close font tags (this probably isn't needed if the above is corret regep)
  $closefonttags = '/<\/FONT>/';
  $nochar = '';
  $body = str_replace($closefonttags, $nochar, $body);
  
  // Remove style comments
  $stylecomments = '/<!--Style:.+?-->/';
  $nochar = '';
  $body = preg_replace($stylecomments, $nochar, $body);
  
  // Add image class
  $pattern = '/IMG|img/';
  $replacement = 'img class="classifiedimage"';
  $body = preg_replace($pattern, $replacement, $body);

  $body = str_replace('""','"',$body);
  
   return $body;
}

/**
 * Remove special characters from text
 */
function _gs_classifieds_strip_characters($text) {
  /*if © strip*/
  $pattern = "/(©)/";
  $replacement = "";
  $text = preg_replace($pattern, $replacement, $text);

  return $text;
}

/**
 * Remove images for teasers
 */
function _gsclassifieds_remove_images($text) {
  /*if image remove it for teaser*/
  $pattern = '/^(\d+).jpg$/';
  $replacement = '';
  $text = preg_replace($pattern, $replacement, $text);

  return $text;
}

/**
 * Add image tags for body
 */
function _gsclassifieds_add_image_tags($text) {
  /*if image add tags before and after*/
  $pattern = '/^(\d+).jpg$/';
  // If the url path of image is ever changed, please update gs_conmio.rss.inc
  // gs_conmio_preprocess_views_view_row_rss() to handle it properly.
  $replacement = '<img src="/files/images/classifieds/thumbnails/${1}.jpg" alt="" />';
  $text = preg_replace($pattern, $replacement, $text);

  return $text;
}

// Convert to CCK-format date
function _gsclassifieds_date_conversion($date, $activation_hour, $days_to_expire) {

  $date = rtrim($date);
  $date_elements = explode("/",$date);
  $hours = 0;
  $minutes = 0;
  $seconds = 0;
  $month = $date_elements[0];
  $day = $date_elements[1];
  $year = $date_elements[2];

  $new_hour = $hours + $activation_hour;
  if (strlen($new_hour) < 2) {
    $new_hour = '0' . $new_hour;
  }  
  
  $activation_timestamp = mktime($new_hour, $minutes, $seconds, $month, $day, $year);
  $expiry_timestamp = $activation_timestamp + $days_to_expire * 24 *60 *60;

//replace the above with $expiry_date = activation date + 7 days. 
// drupal_set_message(date("Y-m-d H:00:00",$activation_timestamp) . " " . date("Y-m-d H:00:00",$expiry_timestamp)) ;
  return array(
    'activation' => date("Y-m-d H:00:00", $activation_timestamp),
    'expiry' => date("Y-m-d H:00:00", $expiry_timestamp)
  );
  /*return array(
    'activation' => date("Y-m-dTH:00:00", $activation_timestamp),
    'expiry' => date("Y-m-dTH:00:00", $expiry_timestamp)
  );*/
}

