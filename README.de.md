FlacShlonz

FlacShlonz ist ein Abgleich-Werkzeug für Musikbibliotheken im FLAC-Format. Es vergleicht zwei Verzeichnisse mit Musikdateien und analysiert die Metadaten jeder Datei.

Beim Start fragt FlacShlonz nach der “Music Collection”, das ist das Verzeichnis in welches neue Daten kopiert werden sollen. Dieses Verzeichnis wird gescant und die Informationen aus jeder FLAC-Datei in einer Datenbank abgelegt.

Anschließend fragt FlacShlonz nach dem “Incoming File Path”, das ist das Verzeichnis aus dem Dateien in die “Music Collection” kopiert werden sollen. Auch dieses Verzeichnis wird gescannt und in der Datenbank abgelegt.

Nach dem Indizieren befindet sich auf der linke Seite des Bildschirms die Ansicht der “Music Collection”, in der Mitte werden die Metadaten einer anklickten Datei angezeigt und auf der rechten Seite die Unterschiede zwischen “Music Collection” und dem “Incoming” Verzeichnis aufgelistet.

FlacShlonz bietet vier Filter-Ansichten für die Unterschiede zwischen “Music Collection” und “Incoming” Verzeichnis. Die Filter-Ansichten können auf der rechten Seite oberhalb der Ergebnisliste umgeschaltet werden:

Tracks you don’t have
Alle FLAC Dateien bei denen weder der Dateiname noch der Audioinhalt in der “Music Collection” vorhanden ist.

Tracks you own, but with different metadata
FLAC Dateien deren Audio-Inhalt in der “Music Collection” vorhanden ist, die jedoch andere Metadaten haben.
(z.B Jahreszahl, Genre, Name des Interpreten, etc.)

Tracks you seem to have with different filename
FLAC Dateien bei denen der Audio-Inhalt in der “Music collection” schon vorhanden ist, jedoch der Dateiname anders ist
(z.B lalala.flac in der “Music collection” aber als lalala1.flac im “Incoming”)

Tracks you maybe have, but in a different named folder
Dateien bei denen der Audio-Inhalt schon vorhanden ist, aber der Name des Verzeichnisses in dem sie liegen ein anderer ist.
(z.B. /Hypnotoad – AllGlory in der “Music collection” aber als /Hypnotoad – Obey Hypnotoad im “Incoming”)

Die gewünschten Files in der jeweiligen Filter-Ansicht markieren und mit der rechten Maustaste auswählen wohin die neuen Dateien kopiert werden sollen. Nun ist die „Music Collection“ auf dem neuesten Stand!

FlacShlonz ist kostenlos, OpenSource und ein Projekt von Musikliebhabern.

Beta Version!

Windows-Variante:
FlacShlonz_28_9_2014.zip

Linux:
FlacShlonz_Linux_09_10_2014.zip
Bitte lesen Sie die Readme_Linux.txt Datei und installieren Sie die darin genannten Komponenten!

Mac: coming soon

Source code: https://github.com/TimSusa/flacshlonz.git

Es handelt sich um eine Betaversion deshalb: Benutzung auf eigene Gefahr!
