<?php

/**
 * Implements whats_on_cinema_integration().
 */
function whats_on_cinema_integration() {
    $cinemas = drupal_http_request('http://www2.cineworld.co.uk/api/quickbook/cinemas?key=YEyQfFvY&full=true');
    $data = drupal_json_decode($cinemas->data);
    foreach ($data as $key => $cinema) {
        foreach ($cinema as $c) {
            whats_on_save_cinema($c , 'cinema');
        }
    }
}

/**
 * @param $title
 * whats_on_check_cinema_exists
 * @return bool -- if films exists returns false
 */
function whats_on_check_cinema_exists($title){
    // Check if movie exists
    $result = db_select('node' , 'n')->fields('n')->condition('title' , $title , '=')->execute()->fetchAssoc();
    if (!empty($result['nid'])) :
        return TRUE;
    else :
        return FALSE;
    endif;

}

/**
 * @param $cinema
 * @param $machine_name
 * @throws Exception
 * Saves cinema as a node.
 */
function whats_on_save_cinema($cinema , $machine_name){
    global $user;
    if (!empty($cinema)) :
        if (!whats_on_check_cinema_exists($cinema['name'])) :
            // Node Content Values
            $node = new stdClass();
            $node->title = 'Cineworld - ' .$cinema['name'];
            $node->type = $machine_name;
            $node->path = array('pathauto' => TRUE);
            $node->field_cinema_adress[LANGUAGE_NONE][0]['value'] = $cinema['address'] . ' ' . $cinema['postcode'];
            $node->field_cinema_telephone_new[LANGUAGE_NONE][0]['value'] = $cinema['telephone'];
            $node->field_cinema_url[LANGUAGE_NONE][0]['value'] = array(
                'value' => $cinema['cinema_url'],
            );
            $node->field_cinema_id[LANGUAGE_NONE][0]['value'] = $cinema['id'];
            node_object_prepare($node); // Sets some defaults. Invokes hook_prepare() and hook_node_prepare().
            $node->language = LANGUAGE_NONE; // Or e.g. 'en' if locale is enabled
            $node->uid = $user->uid;
            $node->status = 1; //(1 or 0): published or not
            $node->promote = 0; //(1 or 0): promoted to front page
            $node->comment = 0; // 0 = comments disabled, 1 = read only, 2 = read/write
            //save the new node
            $node = node_submit($node); // Prepare node for saving
            node_save($node);
            $message = t('Success! Start adding your review for @name', array('@name' => $cinema['name']));
            drupal_set_message( $message );
        endif;
    endif;
}