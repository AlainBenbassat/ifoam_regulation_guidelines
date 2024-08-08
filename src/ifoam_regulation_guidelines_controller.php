<?php

require_once __DIR__ . '/ifoam_regulation_guidelines_api.php';

class IfoamRegulationGuidelinesController {
  public static function process() {
    $action = self::getQueryStringParameter('action', '');
    $params = [
      'email' => self::getQueryStringParameter('email', '')
    ];

    $api = new IfoamRegulationGuidelinesApi();
    return $api->process($action, $params);
  }

  private static function getQueryStringParameter($name, $defaultValue) {
    // get_query_var does not seem to work, so I write my own function

    if (empty($_GET[$name])) {
      return $defaultValue;
    }
    else {
      return $_GET[$name];
    }
  }
}
