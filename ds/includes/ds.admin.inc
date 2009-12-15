<?php
// $Id: ds.admin.inc,v 1.1.2.6 2009/11/28 22:39:15 swentel Exp $

/**
 * @file
 * General file for administrative functions.
 */

/**
 * General page.
 */
function ds_general_page() {

  // Child pages.
  $item = menu_get_item('admin/ds');
  if ($content = system_admin_menu_block($item)) {
    $childs = theme('admin_block_content', $content);
  }
  else {
    $childs = t('You do not have any administrative items.');
  }

  $fieldset = array();
  $fieldset['#title'] = 'General';
  $fieldset['#children'] = $childs;
  $output = theme('fieldset', $fieldset);

  return $output;
}