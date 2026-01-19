<?php

// Prevent direct access via web (comentar se quiser executar pelo navegador)
// if (php_sapi_name() !== 'cli') {
//     die('Este script deve ser executado via linha de comando.');
// }

/**
 * Compile .po in .mo
 */
function compile_mo_file($po_file, $mo_file) {
    if (!file_exists($po_file)) {
        echo "Error: File $po_file not found.\n";
        return false;
    }

    $lines = file($po_file, FILE_IGNORE_NEW_LINES);
    $translations = array();
    $msgid = '';
    $msgstr = '';
    $in_msgid = false;
    $in_msgstr = false;

    foreach ($lines as $line) {
        $line = trim($line);

        if (empty($line) || $line[0] === '#') {
            continue;
        }

        if (preg_match('/^msgid\s+"(.*)"\s*$/', $line, $matches)) {
            if ($msgid !== '' && $msgstr !== '') {
                $translations[$msgid] = $msgstr;
            }
            $msgid = $matches[1];
            $msgstr = '';
            $in_msgid = true;
            $in_msgstr = false;
            continue;
        }
        
        // Detectar msgstr
        if (preg_match('/^msgstr\s+"(.*)"\s*$/', $line, $matches)) {
            $msgstr = $matches[1];
            $in_msgid = false;
            $in_msgstr = true;
            continue;
        }
        
        // Continuar lendo msgid ou msgstr multi-linha
        if (preg_match('/^"(.*)"\s*$/', $line, $matches)) {
            if ($in_msgid) {
                $msgid .= $matches[1];
            } elseif ($in_msgstr) {
                $msgstr .= $matches[1];
            }
        }
    }

    if ($msgid !== '' && $msgstr !== '') {
        $translations[$msgid] = $msgstr;
    }

    $mo_data = pack('Iiiiiii', 0x950412de, 0, count($translations), 28, 28 + count($translations) * 8, 0, 28 + count($translations) * 16);
    
    $keys = array_keys($translations);
    $values = array_values($translations);

    $offset_key = 28 + count($translations) * 16;
    $offset_val = $offset_key;

    foreach ($keys as $key) {
        $offset_val += strlen($key) + 1;
    }

    foreach ($keys as $i => $key) {
        $mo_data .= pack('ii', strlen($key), $offset_key);
        $offset_key += strlen($key) + 1;
    }
    
    foreach ($values as $i => $value) {
        $mo_data .= pack('ii', strlen($value), $offset_val);
        $offset_val += strlen($value) + 1;
    }

    foreach ($keys as $key) {
        $mo_data .= $key . "\0";
    }
    
    foreach ($values as $value) {
        $mo_data .= $value . "\0";
    }

    if (file_put_contents($mo_file, $mo_data) !== false) {
        echo "✓ File generated successfully: $mo_file\n";
        echo "  Total of translations: " . count($translations) . "\n";
        return true;
    } else {
        echo "✗ Erro ao salvar o arquivo $mo_file\n";
        return false;
    }
}

// Executar compilação
echo "=== MO file compiler ===\n\n";

$base_dir = __DIR__;
$files = array(
    'pt_BR' => array(
        'po' => $base_dir . '/pt_BR.po',
        'mo' => $base_dir . '/pt_BR.mo'
    )
);

foreach ($files as $lang => $paths) {
    echo "Compiling $lang...\n";
    compile_mo_file($paths['po'], $paths['mo']);
    echo "\n";
}

echo "=== Compilation finished ===\n";

