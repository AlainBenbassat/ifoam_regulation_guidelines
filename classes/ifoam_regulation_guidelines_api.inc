<?php

class IformaRegulationGuidelinesApi {
  public function __construct() {
    civicrm_initialize();
  }

  public function process($action, $params) {
    $response = new IfoamRegulationGuidelinesApiResponse();

    try {
      if ($action == 'is_member') {
        $this->isMember($response, $params);
      }
      else {
        $this->notImplemented($response, $action, $params);
      }
    }
    catch (Exception $e) {
      $response->status = 'error';
      $response->description = 'an error occurred';
    }

    return $response;
  }

  private function isMember(&$response, $params) {
    $response->is_member = 0;

    if ($params['email'] == '') {
      $response->status = 'error';
      $response->description = 'missing parameter: email';
      return;
    }

    $response->email = $params['email'];

    $contact = new IfoamRegulationGuidelinesContact();
    $contact->lookup($params['email']);

    if ($contact->foundMultiple) {
      $response->status = 'error';
      $response->description = 'found multiple contacts with this email address';
      return;
    }

    if ($contact->foundOne) {
      $response->status = 'success';
      $response->is_member = $contact->isMember;
      $response->description = $contact->details;
      return;
    }

    $response->status = 'error';
    $response->description = 'contact not found';
  }

  private function notImplemented(&$response, $action, $params) {
    $response->status = 'error';

    if ($action == '') {
      $response->description = 'missing parameter: action';
    }
    else {
      $response->description = "action '$action' is not implemented";
    }
  }

}
