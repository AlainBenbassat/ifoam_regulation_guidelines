<?php

class IfoamRegulationGuidelinesContact {
  public $foundOne = FALSE;
  public $foundMultiple = FALSE;
  public $details = 'not a member or sponsor';
  public $isMember = 0;

  private $contactId = 0;
  private $employers = [];

  public function lookup($email) {
    $this->getByEmail($email);

    if (!$this->foundOne) {
      return;
    }

    $this->getEmployers();

    if ($this->isEmployerMember()) {
      $this->isMember = 1;
      return;
    }

    if ($this->isEmployerSponsor()) {
      $this->isMember = 1;
      return;
    }

    $this->details = 'not a member or sponsor';
    $this->isMember = 0;
  }

  private function getByEmail($email) {
    $sql = "
      select
        c.id
      from
        civicrm_contact c
      inner join
        civicrm_email e on e.contact_id = c.id
      where
        e.email = %1
      and
        c.is_deleted = 0
      and
        c.contact_type = 'Individual'
    ";
    $sqlParams = [
      1 => [$email, 'String'],
    ];
    $dao = CRM_Core_DAO::executeQuery($sql, $sqlParams);

    $dao->fetch();

    if ($dao->N == 0) {
      $this->foundOne = FALSE;
      $this->foundMultiple = FALSE;
      return;
    }

    if ($dao->N == 1) {
      $this->foundOne = TRUE;
      $this->foundMultiple = FALSE;
      $this->contactId = $dao->id;
      return;
    }

    $this->foundOne = FALSE;
    $this->foundMultiple = TRUE;
  }

  private function getEmployers() {
    $this->employers = [];

    $relTypeEmployeeOf = 5;
    $relTypeOnTheBoardOf = 21;

    $sql = "
      select distinct
        contact_id_b employer_id
      from
        civicrm_relationship
      where
        is_active = 1
      and
        contact_id_a = {$this->contactId}
      and
        relationship_type_id in ($relTypeEmployeeOf, $relTypeOnTheBoardOf)
    ";

    $dao = CRM_Core_DAO::executeQuery($sql);
    while ($dao->fetch()) {
      $this->employers[] = $dao->employer_id;
    }
  }

  private function isEmployerMember() {
    $membershipGroupIds = '36, 24, 25, 22';
    $membershipDescriptions = [
      36 => 'associate',
      24 => 'full member',
      25 => 'supporting associate',
      22 => 'trial member',
    ];

    foreach ($this->employers as $employer) {
      $sql = "
        select
          gc.group_id,
          c.display_name
        from
          civicrm_group_contact gc
        inner join
          civicrm_contact c on gc.contact_id = c.id
        where
          gc.status = 'Added'
        and
          gc.contact_id = $employer
        and
          gc.group_id in ($membershipGroupIds)
      ";
      $dao = CRM_Core_DAO::executeQuery($sql);
      if ($dao->fetch()) {
        $this->details = $dao->display_name . ' is ' . $membershipDescriptions[$dao->group_id];
        return TRUE;
      }
    }

    return FALSE;
  }

  private function isEmployerSponsor() {
    $membershipGroupId = 631;

    foreach ($this->employers as $employer) {
      $sql = "
        select
          gc.group_id,
          c.display_name
        from
          civicrm_group_contact gc
        inner join
          civicrm_contact c on gc.contact_id = c.id
        where
          gc.status = 'Added'
        and
          gc.contact_id = $employer
        and
          gc.group_id = $membershipGroupId
      ";
      $dao = CRM_Core_DAO::executeQuery($sql);
      if ($dao->fetch()) {
        $this->details = $dao->display_name . ' is sponsor of Regulation Guidelines';
        return TRUE;
      }
    }

    return FALSE;
  }

}
