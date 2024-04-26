<?php

class IfoamRegulationGuidelinesContact {
  public $foundOne = FALSE;
  public $foundMultiple = FALSE;
  public $details = 'not a member or sponsor';
  public $isMember = 0;
  public $isFree = 0;

  private $contactId = 0;
  private $contactName = '';
  private $employers = []; // key = employer id, value = display name

  public function lookup($email) {
    $this->getByEmail($email);

    if (!$this->foundOne) {
      return;
    }

    $this->getEmployers();

    if ($this->isEmployerIfoam()) {
      $this->isMember = 0;
      $this->isFree = 1;
      return;
    }

    if ($this->isFreeAccessContact()) {
      $this->isMember = 0;
      $this->isFree = 1;
      return;
    }

    if ($this->isEmployerSponsor()) {
      $this->isMember = 0;
      $this->isFree = 1;
      return;
    }

    if ($this->isEmployerMember()) {
      $this->isMember = 1;
      $this->isFree = 0;
      return;
    }

    $this->details = 'not a member or sponsor';
    $this->isMember = 0;
  }

  private function getByEmail($email) {
    $contacts = \Civi\Api4\Contact::get(FALSE)
      ->addSelect('*', 'row_count')
      ->addJoin('Email AS email', 'INNER', ['id', '=', 'email.contact_id'])
      ->addWhere('email.email', '=', $email)
      ->addWhere('contact_type', '=', 'Individual')
      ->addWhere('is_deleted', '=', FALSE)
      ->addGroupBy('id')
      ->execute();

    if ($contacts->countMatched() == 0) {
      $this->foundOne = FALSE;
      $this->foundMultiple = FALSE;
      return;
    }

    if ($contacts->countMatched() == 1) {
      $this->foundOne = TRUE;
      $this->foundMultiple = FALSE;

      $this->contactId = $contacts[0]['id'];
      $this->contactName = $contacts[0]['display_name'];
      return;
    }

    $this->foundOne = FALSE;
    $this->foundMultiple = TRUE;
  }

  private function getEmployers() {
    $this->employers = [];

    $relTypeEmployeeOf = 5;
    $relTypeOnTheBoardOf = 21;

    $relationships = \Civi\Api4\Relationship::get(FALSE)
      ->addSelect('contact.*')
      ->addJoin('Contact AS contact', 'INNER', ['contact_id_b', '=', 'contact.id'])
      ->addWhere('contact_id_a', '=', $this->contactId)
      ->addWhere('relationship_type_id', 'IN', [$relTypeEmployeeOf, $relTypeOnTheBoardOf])
      ->addWhere('is_active', '=', TRUE)
      ->addWhere('contact.is_deleted', '=', FALSE)
      ->execute();
    foreach ($relationships as $relationship) {
      $this->employers[$relationship['contact.id']] = $relationship['contact.display_name'];
    }
  }

  private function isEmployerMember() {
    $membershipGroupIds = [36, 24, 25, 22];
    $membershipDescriptions = [
      36 => 'associate member',
      24 => 'full member',
      25 => 'supporting associate member',
      22 => 'trial member',
    ];

    foreach ($this->employers as $employerId => $employerName) {
      $groupContacts = \Civi\Api4\GroupContact::get(FALSE)
        ->addWhere('group_id', 'IN', $membershipGroupIds)
        ->addWhere('contact_id', '=', $employerId)
        ->addWhere('status', '=', 'Added')
        ->execute()
        ->first();

      if ($groupContacts) {
        $this->details = $employerName . ' is ' . $membershipDescriptions[$groupContacts['group_id']];
        return TRUE;
      }
    }

    return FALSE;
  }

  private function isEmployerSponsor() {
    $sponsorsGroupId = 631;

    foreach ($this->employers as $employerId => $employerName) {
      $groupContacts = \Civi\Api4\GroupContact::get(FALSE)
        ->addWhere('group_id', '=', $sponsorsGroupId)
        ->addWhere('contact_id', '=', $employerId)
        ->addWhere('status', '=', 'Added')
        ->execute()
        ->first();

      if ($groupContacts) {
        $this->details = $employerName . ' is sponsor of Regulation Guidelines';
        return TRUE;
      }
    }

    return FALSE;
  }

  private function isEmployerIfoam() {
    $contactIdIFOAM = 1;

    if (array_key_exists($contactIdIFOAM, $this->employers)) {
      $this->details = 'employee or boardmember of IFOAM';
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  private function isFreeAccessContact() {
    $freeAccessGroupId = 635;

    $groupContacts = \Civi\Api4\GroupContact::get(FALSE)
      ->addWhere('group_id', '=', $freeAccessGroupId)
      ->addWhere('contact_id', '=', $this->contactId)
      ->addWhere('status', '=', 'Added')
      ->execute()
      ->first();

    if ($groupContacts) {
      $this->details = $this->contactName . ' is member of the group Developing Organics: REG Free access';
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

}
