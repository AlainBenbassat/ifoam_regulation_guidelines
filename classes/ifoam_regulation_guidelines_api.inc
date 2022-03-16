<?php

class IformaRegulationGuidelinesApi {
  public function __construct() {
    civicrm_initialize();
  }

  public function process($action, $params) {
    $response = new IfoamRegulationGuidelinesApiResponse();

    try {
      if ($action == 'guidelines_access') {
        $this->checkGuidelinesAccess($response, $params);
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

  private function checkGuidelinesAccess(&$response, $params) {
    if ($this->isMissingEmail($response, $params)) {
      return;
    }

    $contact = new IfoamRegulationGuidelinesContact();
    $contact->lookup($params['email']);

    if ($contact->foundMultiple) {
      $response->status = 'error';
      $response->description = 'found multiple contacts with this email address';
      return;
    }

    if ($contact->foundOne) {
      $response->status = 'success';
      $response->description = $contact->details;
      $response->is_member = $contact->isMember;
      $response->is_free = $contact->isFree;
      return;
    }

    $response->status = 'success';
    $response->description = 'contact not found';
  }

  private function isMissingEmail(&$response, $params) {
    if ($params['email'] == '') {
      $response->status = 'error';
      $response->description = 'missing parameter: email';
      return TRUE;
    }
    else {
      $response->email = $params['email'];
      return FALSE;
    }
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
