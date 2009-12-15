<?php
// $Id: ds.registry.inc,v 1.1.2.11 2009/11/28 22:39:15 swentel Exp $

/**
 * @file
 * Registry functions.
 */

/**
 * Return menu items.
 */
function _ds_menu() {
  $items = array();
  $all_build_modes = ds_get_build_modes(NULL, TRUE);

  // Display suite main menu block.
  $items['admin/ds'] = array(
    'title' => 'Display suite',
    'description' => 'Manage your site display.',
    'page callback' => 'ds_general_page',
    'access arguments' => array('administer content types'),
    'file' => 'includes/ds.admin.inc',
  );

  // Import / Export
  $items['admin/ds/tools'] = array(
    'title' => 'Tools',
    'description' => 'Tools for Display suite',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ds_import'),
    'access arguments' => array('administer content types'),
    'file' => 'includes/ds.tools.inc',
    'weight' => -1,
  );
  $items['admin/ds/tools/import'] = array(
    'title' => 'Import',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 0,
  );
  $items['admin/ds/tools/export'] = array(
    'title' => 'Export',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('ds_export'),
    'access arguments' => array('administer content types'),
    'file' => 'includes/ds.tools.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 1,
  );

  // Display layout menu item.
  $items['admin/ds/layout'] = array(
    'title' => 'Layout',
    'page callback' => 'ds_layout_general',
    'access arguments' => array('administer content types'),
    'file' => 'includes/ds.display.inc',
    'weight' => 2,
  );

  foreach (module_implements('ds_api') as $module) {
    $function = $module .'_ds_api';
    $info = $function();
    $module = $info['module'];
    $title = $info['title'];

    // Info.
    $items['admin/ds/'. $module] = array(
      'title' => $title,
      'page callback' => 'ds_info',
      'page arguments' => array($module),
      'access arguments' => array('administer content types'),
      'file' => 'includes/ds.info.inc',
    );
    $items['admin/ds/'. $module .'/info'] = array(
      'title' => 'Info',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => 0,
    );

    // Fields.
    $items['admin/ds/'. $module .'/fields'] = array(
      'title' => 'Fields',
      'page callback' => 'ds_fields',
      'page arguments' => array($module),
      'access arguments' => array('administer content types'),
      'file' => 'includes/ds.fields.inc',
      'type' => MENU_LOCAL_TASK,
      'weight' => 1,
    );

    // Build modes.
    $items['admin/ds/'. $module .'/buildmodes'] = array(
      'title' => 'Build modes',
      'page callback' => 'ds_build_modes',
      'page arguments' => array($module),
      'access arguments' => array('administer content types'),
      'file' => 'includes/ds.buildmodes.inc',
      'type' => MENU_LOCAL_TASK,
      'weight' => 2,
    );

    // Plugins.
    $items['admin/ds/'. $module .'/plugins'] = array(
      'title' => 'Plugins',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('ds_plugins', $module),
      'access arguments' => array('administer content types'),
      'file' => 'includes/ds.plugins.inc',
      'type' => MENU_LOCAL_TASK,
      'weight' => 3,
    );

    // Displays.
    $build_modes = $all_build_modes[$module];
    if (!empty($build_modes)) {
      foreach ($info['types']() as $tkey => $type) {
        $type_name = $type->type;
        $type_url_str = str_replace('_', '-', $type_name);

        $items['admin/ds/layout/'. $type_url_str] = array(
          'title' => drupal_ucfirst($type_url_str),
          'page callback' => 'drupal_get_form',
          'page arguments' => array('ds_display_overview_form', $type_name, 'full', $module),
          'access arguments' => array('administer content types'),
          'file' => 'includes/ds.display.inc',
          'weight' => 2,
        );

        $weight = 20;
        foreach ($build_modes as $key => $value) {
          $items['admin/ds/layout/'. $type_url_str .'/'. $key] = array(
            'title' => $value['title'],
            'page callback' => 'drupal_get_form',
            'page arguments' => array('ds_display_overview_form', $type_name, "$key", $module),
            'access arguments' => array('administer content types'),
            'type' => $key == 'full' ? MENU_DEFAULT_LOCAL_TASK : MENU_LOCAL_TASK,
            'file' => 'includes/ds.display.inc',
            'weight' => (isset($value['weight']) && !empty($value['weight'])) ? $value['weight'] : $weight++,
          );
        }
      }
    }
  }

  return $items;
}

/**
 * Return theme functions.
 */
function _ds_theme() {
  $path = drupal_get_path('module', 'ds');

  $theme_functions = array(
    // Display overview form.
    'ds_display_overview_form' => array(
      'template' => 'ds-display-overview-form',
      'file' => 'theme.inc',
      'path' => $path .'/theme',
      'arguments' => array('form' => NULL),
    ),
    // Build modes matrix.
    'ds_buildmodes_matrix_form' => array(
      'file' => 'ds.buildmodes.inc',
      'path' => $path .'/includes',
      'arguments' => array('form' => NULL),
    ),
    // Plugins form.
    'ds_plugins' => array(
      'file' => 'ds.plugins.inc',
      'path' => $path .'/includes',
      'arguments' => array('form' => NULL),
    ),

    // General theming field.
    'ds_field' => array(
      'arguments' => array(
        'content' => NULL,
        'field_key' => NULL,
        'field' => NULL,
      ),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
    ),


    // Theming for fields which are shared across objects (name, picture etc).
    'ds_author_link' => array(
      'arguments' => array('object' => NULL),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
    ),
    'ds_author_nolink' => array(
      'arguments' => array('object' => NULL),
      'file' => 'theme.inc',
      'path' => $path .'/theme',
    ),
  );

  return $theme_functions;
}

/**
 * Register build modes.
 */
function _ds_register_build_modes() {

  // Build modes defined in hooks.
  $weight = 10;
  $build_modes = array();
  foreach (module_implements('content_build_modes') as $module) {
    $module_data = array();
    $function = $module .'_content_build_modes';
    $temp_data = $function();
    // We need to do some fiddling existing build modes.
    foreach ($temp_data as $key => $value) {
      // Sane build modes.
      $module_name = isset($value['module']) ? $value['module'] : 'nd';
      $build_modes[$module_name][$key] = array(
        'weight' => isset($value['weight']) ? $value['weight'] : $weight++,
        'title' => $value['title'],
      );
    }
  }

  // Custom build modes via the UI.
  foreach (module_implements('ds_api') as $module) {
    $weight = 10;
    $db_build_modes = variable_get($module .'_build_modes', array());
    if (!empty($db_build_modes)) {
      foreach ($db_build_modes as $key => $name) {
        $build_modes[$module][$key] = array(
          'weight' => $weight++,
          'title' => check_plain($name),
        );
      }
    }
  }

  // Give modules a change to alter build_modes.
  drupal_alter('ds_buildmodes', $build_modes);

  // Cache all build modes.
  variable_set('ds_all_build_modes', $build_modes);

  return $build_modes;
}

/**
 * Return nd plugins.
 */
function _ds_plugins() {
  $path = drupal_get_path('module', 'ds');
  return array(
    'emptyregionrender' => array(
      'title' => t('Empty region'),
      'description' => t('Renders a region when there is no content in it.'),
      'file' => 'emptyregionrender.inc',
      'path' => $path .'/plugins',
      'target' => 'all',
    ),
    'cssoverrider' => array(
      'title' => t('Simple CSS overrider'),
      'description' => t('Override region CSS with inline styles or add extra classes.'),
      'file' => 'cssoverrider.inc',
      'path' => $path .'/plugins',
      'target' => 'all',
    ),
  );
}
