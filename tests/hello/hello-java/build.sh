#!/bin/sh

javac hello.java
echo '#!/bin/sh' > run
echo 'java hello' >> run