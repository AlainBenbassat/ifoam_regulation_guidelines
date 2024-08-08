<?php
/**
* Plugin Name: IFOAM Regulations Guidelines
* Plugin URI: https://github.com/AlainBenbassat/ifoam_regulation_guidelines
* Description: Checks permissions for the IFOAM Regulations Guidelines site and returns JSON
* Version: 3.0
* Author: Alain Benbassat
* Author URI: https://www.businessandcode.eu/
**/

if ( ! defined( 'ABSPATH' ) ) {
  exit; // Exit if accessed directly
}

function ifoam_regulation_guidelines_api() {
  require_once __DIR__ . '/src/ifoam_regulation_guidelines_controller.php';
  $response = IfoamRegulationGuidelinesController::process();
  wp_send_json($response);
}

add_shortcode('regulation-guidelines-api', 'ifoam_regulation_guidelines_api');

