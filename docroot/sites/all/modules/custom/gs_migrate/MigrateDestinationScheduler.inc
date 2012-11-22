<?php
/**
 * @file
 * Support for the Scheduler module.
 */

/**
 * Field handler.
 */
class MigrateSchedulerHandler extends MigrateDestinationHandler {
  public function __construct() {
    $this->registerTypes(array('node'));
  }

  /**
   * Make the destination field visible.
   */
  public function fields() {
    return array(
      'scheduler' =>
        t('Scheduler: scheduler_publish_on  publishing date for scheduler'),
    );
  }

  public function prepare($entity, stdClass $row) {
    if (isset($row->scheduler_publish_on)) {

      $entity->status = 0;
      $entity->publish_on = $row->scheduler_publish_on;
    }
  }
}