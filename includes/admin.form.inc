<?php

/**
 * @file
 * Handles the display/submission of the admin settings form for this module.
 */

/**
 * Defines the admin settings form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_paged_content_admin_settings_form(array $form, array &$form_state) {
  form_load_include($form_state, 'inc', 'islandora_paged_content', 'includes/admin.form');
  $get_default_value = function($name, $default) use(&$form_state) {
    return isset($form_state['values'][$name]) ? $form_state['values'][$name] : variable_get($name, $default);
  };
  $gs = $get_default_value('islandora_paged_content_gs', '/usr/bin/gs');

  $djatoka_url = $get_default_value('islandora_paged_content_djatoka_url', 'http://localhost:8080/adore-djatoka/');
  $djatoka_availible_message = islandora_paged_content_admin_settings_form_djatoka_availible_message($djatoka_url);
  $solr_enabled = module_exists('islandora_solr');

  $form = array(
    'pdf_derivative_settings' => array(
      '#type' => 'fieldset',
      '#title' => t('PDF Derivative Settings'),
      'islandora_paged_content_gs' => array(
        '#type' => 'textfield',
        '#title' => t('gs (GhostScript)'),
        '#description' => t('GhostScript is used to combine PDF files into a representation of a book or newspaper.<br/>!msg',
                        array('!msg' => islandora_paged_content_admin_settings_form_executable_available_message($gs))),
        '#default_value' => $gs,
        '#prefix' => '<div id="gs-wrapper">',
        '#suffix' => '</div>',
        '#ajax' => array(
          'callback' => 'islandora_paged_content_admin_settings_form_gs_ajax_callback',
          'wrapper' => 'gs-wrapper',
          'effect' => 'fade',
          'event' => 'change',
        ),
      ),
    ),
    'islandora_paged_content_djatoka_url' => array(
      '#type' => 'textfield',
      '#prefix' => '<div id="djatoka-path-wrapper">',
      '#suffix' => '</div>',
      '#title' => t('djatoka URL'),
      '#description' => t('URL to the djatoka instance.<br/>!@msg', array('@msg' => $djatoka_availible_message)),
      '#default_value' => $djatoka_url,
      '#ajax' => array(
        'callback' => 'islandora_paged_content_admin_settings_form_djatoka_ajax_callback',
        'wrapper' => 'djatoka-path-wrapper',
        'effect' => 'fade',
        'event' => 'change',
      ),
    ),
    'islandora_paged_content_sequence_number_field' => array(
      '#access' => $solr_enabled,
      '#type' => 'textfield',
      '#title' => t('Solr page sequence number field'),
      '#description' => t('The page or sequence number of each page or image.'),
      '#default_value' => $get_default_value('islandora_paged_content_sequence_number_field', 'RELS_EXT_isSequenceNumber_literal_ms'),
    ),
  );
  return system_settings_form($form);
}

/**
 * Gets a message to display if the ocr program provided is accessible or not.
 *
 * @param string $path
 *   The path to an executable to check for availability.
 * @param string $version
 *   The version of exectuable.
 *
 * @return string
 *   A message in html detailing if the given executable is accessible.
 */
function islandora_paged_content_admin_settings_form_executable_available_message($path, $version = NULL, $required_version = NULL) {
  $available = is_executable($path);
  if ($available) {
    $image = theme_image(array('path' => 'misc/watchdog-ok.png', 'attributes' => array()));
    $message = t('Executable found at @path', array('@path' => $path));
    if ($version) {
      $message .= t('<br/>Version: @version', array('@version' => $version));
    }
    if ($required_version) {
      $message .= t('<br/>Required Version: @version', array('@version' => $required_version));
      if (version_compare($version, $required_version) < 0) {
        $image = theme_image(array('path' => 'misc/watchdog-error.png', 'attributes' => array()));
      }
    }
  }
  else {
    $image = theme_image(array('path' => 'misc/watchdog-error.png', 'attributes' => array()));
    $message = t('Unable to locate executable at @path', array('@path' => $path));
  }
  return $image . $message;
}

/**
 * Ajax callback for the GS textfield.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The element to render as part the ajax callback.
 */
function islandora_paged_content_admin_settings_form_gs_ajax_callback(array $form, array $form_state) {
  return $form['local_derivative_settings']['pdf_derivative_settings']['islandora_book_gs'];
}

/**
 * Gets a message to display if the djatoka is accessible or not.
 *
 * @param string $djatoka_url
 *   The url to an djatoka instance typically
 *   http://localhost:8080/adore-djatoka.
 *
 * @return string
 *   A message in html detailing if the djatoka is accessible.
 */
function islandora_paged_content_admin_settings_form_djatoka_availible_message($djatoka_url) {
  $file_headers = @get_headers($djatoka_url);
  $djatoka_availible = strstr($file_headers[0], '200') !== FALSE;
  if ($djatoka_availible) {
    $image = theme_image(array('path' => 'misc/watchdog-ok.png', 'attributes' => array()));
    $message = t('djatoka url is valid.');
  }
  else {
    $image = theme_image(array('path' => 'misc/watchdog-error.png', 'attributes' => array()));
    $message = t('Unable to locate djatoka installation at @path', array('@path' => $djatoka_url));
  }
  return $image . $message;
}

/**
 * Ajax callback for the djatoka url textfield.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The element to render as part the ajax callback.
 */
function islandora_paged_content_admin_settings_form_djatoka_ajax_callback(array $form, array &$form_state) {
  return $form['islandora_paged_content_djatoka_url'];
}
