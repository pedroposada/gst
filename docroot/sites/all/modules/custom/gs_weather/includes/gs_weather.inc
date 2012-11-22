<?php

/*
 * @file
 *
 * Gets weather from Environment Canada. Checks first in database and if weather is old, then gets it fresh from RSS feed
 * 
 * $type = Either "page" or "block". "page" returns content for a whole weather page. "block" returns content for a tiny weather widget block.
 * 
 * Returns rendered HTML output of full weather page content.
 */
function gscontent_get_environmentCanada_weather($type = "page") {
  global $theme;
  
  // First get weather RSS data from database as an XML object
  $data_feedxml = gscontent_get_environment_canada_weather_from_database();

  if ($data_feedxml == FALSE) {
    // Database is old or blank so get fresh RSS data from feed as an XML object
    $data_feedxml = gscontent_get_environmentCanada_weather_from_rss();
  }
  if ($data_feedxml == FALSE) {
    // Something went really wrong.
    return "Error in fetching weather from Environment Canada.";
  }

  $wind_gust = array();
  $units = 'N/A';
  foreach ($data_feedxml->currentConditions->wind->gust[0]->attributes() as $k => $v) {
    if($k == 'units') {
    $units = $v;
  }
  }
  $tendency = 'N/A';
  foreach($data_feedxml->currentConditions->pressure[0]->attributes() as $a => $b) {
  if($a == 'tendency') {
    $tendency = $b;
  }
  }

  // Now we should have a valid (timely) weather object in simple_xml format
  $weather_time_of_capture_array = explode("at", $data_feedxml->dateTime[1]->textSummary);
  $weatherDateTimestamp = strtotime($weather_time_of_capture_array[0] . $weather_time_of_capture_array[1]);
  $weatherArray = array(
    'timestamp' => $weatherDateTimestamp,
    'timePeriod' => "Current Conditions",
    'location' => sprintf("%s", $data_feedxml->location->region),
    'pressure' => sprintf("%s", $data_feedxml->currentConditions->pressure),
    'visibility' => sprintf("%s", $data_feedxml->currentConditions->visibility),
    'dewpoint' => sprintf("%s", $data_feedxml->currentConditions->dewpoint),
    'relativeHumidity' => sprintf("%s", $data_feedxml->currentConditions->relativeHumidity),
    'tendency' => sprintf("%s", $tendency),
    'windSpeed' => sprintf("%s", $data_feedxml->currentConditions->wind->direction) . " " . sprintf("%s", $data_feedxml->currentConditions->wind->speed) . " " . sprintf("%s", $units),
    'station' => sprintf("%s", $data_feedxml->currentConditions->station),
    'descriptionShort' => sprintf("%s", $data_feedxml->currentConditions->condition),
    'descriptionLong' => "",
    'temperature' => sprintf("%s", $data_feedxml->currentConditions->temperature),
    'iconCode' => sprintf("%s", $data_feedxml->currentConditions->iconCode),
  );

  if ($type == "block") {
    $count = 0;
    $output  = '<a href="/weather" class="weather_widget_link">';
    $output .= '<div class="weather_widget_block">';
    $output .= '<div class="current_weather">';
    $output .= gscontent_format_weather_widget($weatherArray);
    $output .= '</div>';
    $today = date('l');

    foreach ($data_feedxml->forecastGroup->forecast as $aWeatherReport) {
      if ($count < 3 && !strpos($aWeatherReport->period, 'night') && ($aWeatherReport->period != $today)) {
        
        $fahrenheit = ($aWeatherReport->temperatures->temperature * 1.8) + 32;
        $weatherPeriod = $aWeatherReport->period;
        $weatherPeriodArray = explode(" ", $weatherPeriod);
  
    if (sizeof($weatherPeriodArray) > 1) {
          $timestamp = strtotime($weatherPeriodArray[0]);
          $extraDate = $weatherPeriodArray[1];
        } 
        else {
          $timestamp = strtotime($weatherPeriod);
          $extraDate = "";
        }
      
        $weatherArray = array(
          'timestamp' => $timestamp,
          'timePeriod' => sprintf("%s", $aWeatherReport->period),
          'location' => sprintf("%s", $data_feedxml->location->region),
          'station' => sprintf("%s", $data_feedxml->currentConditions->station),
          'descriptionShort' => sprintf("%s", $aWeatherReport->abbreviatedForecast->textSummary),
          'descriptionLong' => sprintf("%s", $aWeatherReport->textSummary),
          'temperature' => sprintf("%s", $aWeatherReport->temperatures->temperature),
          'temperatureFahrenheit' => sprintf("%s", $fahrenheit),
          'iconCode' => sprintf("%s", $aWeatherReport->abbreviatedForecast->iconCode)
        );
        $output .= '<div class="forecast_weather">';
        $output .= gscontent_format_weather_widget($weatherArray);
        $output .= '</div>';
        $count++;
      }
    }
    $output .= '</div>';
    $output .= '</a>';
    return $output;
  }
  
  // Format the new rss data (and store it in the DB)
  $output  = '<div class="weather_page_content">';
  $output .= '<div class="weather_page_current_content">';
  $output .= '<div class="weather_current_header">Current Conditions</div>';
  $output .= gscontent_format_weather_current_conditions($weatherArray);
  $output .= '</div>';
  $output .= '<div class="weather_forecast_header">Forecast</div>';
  
  foreach ($data_feedxml->forecastGroup->forecast as $aWeatherReport) {
    $fahrenheit = ($aWeatherReport->temperatures->temperature * 1.8) + 32;
    $weatherPeriod = $aWeatherReport->period;
    $weatherPeriodArray = explode(" ", $weatherPeriod);
    if (sizeof($weatherPeriodArray) > 1) {
      $timestamp = strtotime($weatherPeriodArray[0]);
      $extraDate = $weatherPeriodArray[1];
    } 
    else {
      $timestamp = strtotime($weatherPeriod);
      $extraDate = "";
    }
  
    $weatherArray = array(
      'timestamp' => $timestamp,
      'timePeriod' => sprintf("%s", $aWeatherReport->period),
      'location' => sprintf("%s", $data_feedxml->location->region),
      'station' => sprintf("%s", $data_feedxml->currentConditions->station),
      'descriptionShort' => sprintf("%s", $aWeatherReport->abbreviatedForecast->textSummary),
      'descriptionLong' => sprintf("%s", $aWeatherReport->textSummary),
      'temperature' => sprintf("%s", $aWeatherReport->temperatures->temperature),
      'temperatureFahrenheit' => sprintf("%s", $fahrenheit),
      'iconCode' => sprintf("%s", $aWeatherReport->abbreviatedForecast->iconCode)
    );
      
    $output .= gscontent_format_weather($weatherArray);
  }
  
  $iconPath = "/" . drupal_get_path('module', 'gs_weather') . "/img/";
  $output .= '<div style="clear: both;"></div>';
  $output .= '<div class="weather_footer">';
  $output .= 'Based on <a href="http://www.weatheroffice.gc.ca/city/pages/bc-74_metric_e.html">Environment Canada data collected at the '. $weatherArray['station'] . '</a> on <br>' . $data_feedxml->dateTime[1]->textSummary . '<br>';
  $output .= '<a href="http://www.weatheroffice.gc.ca/city/pages/bc-74_metric_e.html"><img src="' . $iconPath . 'environmentCanadaLogo.jpg" border="0" style="margin-top: 3px;"></a>';
  $output .= '</div>';
    
  $output .= '</div class="weather_block">';
  
  return $output; 
}

/*
 * Fetches weather data from RSS feed. Updates database with new data
 * 
 * Returns simple_xml rss object or FALSE if failed.
 */
function gscontent_get_environmentCanada_weather_from_rss() {
  $data_feedxml = "http://dd.weatheroffice.gc.ca/citypage_weather/xml/BC/s0000141_e.xml"; 

  // create a new cURL resource
  $ch = curl_init();

  // set URL and other appropriate options
  curl_setopt($ch, CURLOPT_URL, $data_feedxml);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER , TRUE);
  curl_setopt($ch, CURLOPT_HEADER, FALSE);
  curl_setopt($ch, CURLOPT_TIMEOUT, 20);

  // grab URL and pass it to the browser
  $result = curl_exec($ch);

  // close cURL resource, and free up system resources
  curl_close($ch);
  
  $timestamp = mktime();
  
  if ($result != FALSE) {
    $data_feedxml = simplexml_load_string($result);

    // Remove all old weather data from the DB
    $query = "DELETE FROM {weather_rss_data} WHERE source='Environment Canada'";
    $results = db_query($query);
    
    // Add the latest rss string to our database
    $result = str_replace("<", "&lt;", $result);
    $result = str_replace(">", "&gt;", $result);
    $result = str_replace("'", "\'", $result);
    
    $query = "INSERT INTO {weather_rss_data} (source, timestamp, rss) VALUES (:n, :t, :r) ON DUPLICATE KEY UPDATE rss=:re, timestamp=:te";
    $dbresult = db_query($query, array(':n'=>'Environment Canada', ':t'=>$timestamp, ':r'=>$result, ':re'=>$result, ':te'=>$timestamp));
    
    return $data_feedxml;
  } 
  else {
    return FALSE;
  }
}

/*
 * Returns simple_xml RSS object from database
 * OR
 * FALSE if timestamp of RSS object is over 1 hour old
 * OR
 * FALSE if there is no RSS object in the database
 */
function gscontent_get_environment_canada_weather_from_database() {
  $maxAgeOfWeather = 20; // in minutes
  
  $query = "SELECT * FROM {weather_rss_data} WHERE source = 'Environment Canada'";
  $results = db_query($query);
  $numOfResults = $results->rowCount();
  
  if ($numOfResults == 0 || $results == FALSE) {
    return FALSE;
  } 
  else {
     while ($row = $results->fetchAssoc()) {
      $source = $row['source'];
      $timestamp = $row['timestamp'];
      $xmlString = str_replace("&lt;", "<", $row['rss']);
      $xmlString = str_replace("&gt;", ">", $xmlString);
      $xmlString = str_replace("\'", "'", $xmlString);
    $rssAsXML = simplexml_load_string($xmlString);
      $weather_time_of_capture_array = explode("at", $rssAsXML->dateTime[1]->textSummary);
      $timestampOfRSS = strtotime($weather_time_of_capture_array[0] . $weather_time_of_capture_array[1]);
      $hmsOfRSS = date("h:i:s", $timestampOfRSS);
      $hmsOfNow = date("h:i:s");
      $mins_old = (mktime() - $timestampOfRSS) / 60;
      if ($mins_old > $maxAgeOfWeather) {
        return FALSE;
      } 
      else {
        return $rssAsXML;
      }
    }
  }
}

/*
 * Formats weather information for display in html
 * 
 * Params:
 * $date - String in format: YYYY-MM-DD
 * $description - String describing weather (eg 'Partly Cloudy')
 * $temperature - String describing tempertature (eg 'High 9&nbsp;&deg;C')
 */
function gscontent_format_weather($weatherArray) {
  $date = date("F j, Y", $weatherArray['timestamp']);
  $iconPath = "/" . drupal_get_path('module', 'gs_weather') . "/img/";
  
  $output  = '<div class="weather_detail">';
  $output .= '<div class="weather_date"><span class="weather-day">' . $weatherArray['timePeriod'] . '</span><span class="weather-date">' . $date . '</span></div>';
  $output .= '<div class="weather_icon"><img src="' . $iconPath . $weatherArray['iconCode'] . '.png"></div>';
  $output .= '<div class="weather_temp_body"><span class="weather-celsius">' . round($weatherArray['temperature']) . '&deg;C</span>/<span class="weather-fahrenheit">' . round($weatherArray['temperatureFahrenheit']) . '&deg;F</span></div>';
  $output .= '<div class="weather_description_body_long">' . $weatherArray['descriptionLong'] . '</div>';
  $output .= '</div>';
  
  return $output;
}

function gscontent_format_weather_current_conditions($weatherArray) {
  $date = date("F j, Y", $weatherArray['timestamp']);
  $iconPath = "/" . drupal_get_path('module', 'gs_weather') . "/img/";
  
  $output = '<div class="weather_date">' . $date . '</div>';
  $output .= '<div class="weather_detail_current_conditions">';
  $output .= '<div class="weather_first_column">';
  $output .= '<div class="weather_icon"><img src="' . $iconPath . $weatherArray['iconCode'] . '.png"></div>';
  $output .= '<div class="weather_temp_body">' . round($weatherArray['temperature']) . '&deg;C</div>';
  $output .= '</div>';
  $output .= '<div class="weather_second_column">';
  $output .= '<div class="weather_item"><span class="column-label">' . t("Condition") . ':</span><span class="column-text">' . $weatherArray['descriptionShort'] . '</span></div>';
  $output .= '<div class="weather_item"><span class="column-label">' . t("Pressure") . ':</span><span class="column-text">' . $weatherArray['pressure'] . '</span></div>';
  $output .= '<div class="weather_item"><span class="column-label">' . t("Tendency") . ':</span><span class="column-text">' . $weatherArray['tendency'] . '</span></div>';
  $output .= '<div class="weather_item"><span class="column-label">' . t("Visibility") . ':</span><span class="column-text">' . $weatherArray['visibility'] . '</span></div>';
  $output .= '</div>';
  $output .= '<div class="weather_third_column">';
  $output .= '<div class="weather_item"><span class="column-label">' . t("Temperature") . ':</span><span class="column-text">' . $weatherArray['temperature'] . '</span></div>';
  $output .= '<div class="weather_item"><span class="column-label">' . t("Dewpoint") . ':</span><span class="column-text">' . $weatherArray['dewpoint'] . '</span></div>';
  $output .= '<div class="weather_item"><span class="column-label">' . t("Humidity") . ':</span><span class="column-text">' . $weatherArray['relativeHumidity'] . '</span></div>';
  $output .= '<div class="weather_item"><span class="column-label">' . t("Wind") . ':</span><span class="column-text">' . $weatherArray['windSpeed'] . '</span></div>';
  $output .= '</div>';
  $output .= '</div>';
  
  return $output;
  
}

function gscontent_format_weather_widget($weatherArray) {
  $iconPath = "/" . drupal_get_path('module', 'gs_weather') . "/img/";
  
  $timePeriod = $weatherArray['timePeriod'];
  $class = "forecast";
  if ($timePeriod == "Current Conditions") {
    $timePeriod = "Today &raquo;";
    $class = "current";
  }
  else {
    $long = array("Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday");
    $abreviation   = array("Mon", "Tues", "Wed", "Thurs", "Fri", "Sat", "Sun");
    $timePeriod = str_replace($long, $abreviation, $timePeriod);
  }
  $output = '<div class="' . $class . '_weather_title">' . $timePeriod . '</div>';
  $output .= '<div class="' . $class . '_weather_icon"><img src="' . $iconPath . $weatherArray['iconCode'] . '.png" alt="' . $weatherArray['descriptionShort'] . '"></div>';
  $output .= '<div class="' . $class . '_weather_temp_body">' . round($weatherArray['temperature']) . '&deg;C</div>';
  
  return $output;
}

