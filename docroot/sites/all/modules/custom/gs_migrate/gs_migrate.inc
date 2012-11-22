<?php

/**
 * There are four essential components to set up in your constructor:
 *  $this->source - An instance of a class derived from MigrateSource, this
 *    will feed data to the migration.
 *  $this->destination - An instance of a class derived from MigrateDestination,
 *    this will receive data that originated from the source and has been mapped
 *    by the Migration class, and create Drupal objects.
 *  $this->map - An instance of a class derived from MigrateMap, this will keep
 *    track of which source items have been imported and what destination objects
 *    they map to.
 *  Mappings - Use $this->addFieldMapping to tell the Migration class what source
 *    fields correspond to what destination fields, and additional information
 *    associated with the mappings.
 */
abstract class GSMigration extends Migration {
  public function __construct() {
    // Always call the parent constructor first for basic setup
    parent::__construct(MigrateGroup::getInstance('gsmigrate'));

    // With migrate_ui enabled, migration pages will indicate people involved in
    // the particular migration, with their role and contact info. We default the
    // list in the shared class; it can be overridden for specific migrations.
    $this->team = array(
      new MigrateTeamMember('Pedro Posada', 'p_posada@straight.com', t('Developer')),
			new MigrateTeamMember('Josh Solomon', 'j_solomon@straight.com', t('Developer')),
    );

	//  connect to d6 database
	Database::addConnectionInfo('d6', 'default', $GLOBALS['databases']['d6']['default']);
	
  }
}


/**
 * TIP: While usually you'll create true migrations - processes that copy data
 * from some source into Drupal - you can also define processing steps for either
 * the import or rollback stages that take other actions. In this case, we want
 * to disable auto_nodetitle while the migration steps run.
 */
// class GSPrepMigration extends MigrationBase {
  // // Remember whether the auto_nodetitle was originally enabled, so we know whether
  // // to re-enable it
  // public static $wasEnabled = FALSE;
// 
  // public function __construct() {
    // // Because we're derived directly from migrationBase rather than AdvancedExampleMigration,
    // // we must specify the group again here.
    // parent::__construct(MigrateGroup::getInstance('gsmigrate'));
    // $this->description = t('If xmlsitemap and gs_helper are present, disable them for the duration');
  // }
  // // Define isComplete(), returning a boolean, to indicate whether dependent
  // // migrations may proceed
  // public function isComplete() {
    // // If xmlsitemap Title is disabled, other migrations are free to go
    // if (module_exists('xmlsitemap')) {
      // return FALSE;
    // }
    // else {
      // return TRUE;
    // }
    // if (module_exists('gs_helper')) {
      // return FALSE;
    // }
    // else {
      // return TRUE;
    // }
  // }
  // // Implement any action you want to occur during an import process in an
  // // import() method (alternatively, if you have an action which you want to
  // // run during rollbacks, define a rollback() method).
  // public function import() {
    // if (module_exists('filter')) {
      // self::$wasEnabled = TRUE;
      // module_disable(array('filter'));
      // $this->showMessage(t('Disabled filter module'), 'success');
    // }
    // else {
      // self::$wasEnabled = FALSE;
      // $this->showMessage(t('filter is already disabled'), 'success');
    // }
    // if (module_exists('gs_helper')) {
      // self::$wasEnabled = TRUE;
      // module_disable(array('gs_helper'));
      // $this->showMessage(t('Disabled gs_helper module'), 'success');
    // }
    // else {
      // self::$wasEnabled = FALSE;
      // $this->showMessage(t('gs_helper is already disabled'), 'success');
    // }
    // // Must return one of the MigrationBase RESULT constants
    // return MigrationBase::RESULT_COMPLETED;
  // }
  // public function rollback() {
    // if (module_exists('filter')) {
      // self::$wasEnabled = TRUE;
      // module_enable(array('filter'));
      // $this->showMessage(t('Enabled filter module.'), 'success');
    // }
    // else {
      // self::$wasEnabled = FALSE;
      // $this->showMessage(t('filter does not exist.'), 'success');
    // }
    // if (module_exists('gs_helper')) {
      // self::$wasEnabled = TRUE;
      // module_enable(array('gs_helper'));
      // $this->showMessage(t('Enabled gs_helper module.'), 'success');
    // }
    // else {
      // self::$wasEnabled = FALSE;
      // $this->showMessage(t('gs_helper does not exist.'), 'success');
    // }
    // // Must return one of the MigrationBase RESULT constants
    // return MigrationBase::RESULT_COMPLETED;
  // }
// }