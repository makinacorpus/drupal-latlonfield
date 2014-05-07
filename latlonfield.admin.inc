<?php

/**
 * Generate the settings form.
 */
function latlonfield_settings_form($form, &$form_state) {

  $form['latlonfield_google_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Google map API key'),
    '#default_value' => variable_get('latlonfield_google_api_key', ''),
  );

  return system_settings_form($form);
}
