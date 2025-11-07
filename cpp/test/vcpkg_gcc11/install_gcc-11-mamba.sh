#!/usr/bin/env bash
set -euo pipefail

dnf install mamba -y
mamba create -n gcc11 -c conda-forge gcc_linux-64=11 gxx_linux-64=11 -y
ENV="/envs/gcc11"

tee /usr/bin/gcc-11 >/dev/null <<SH
#!/usr/bin/env bash
exec "$ENV/bin/x86_64-conda-linux-gnu-gcc" "\$@"
SH
sudo chmod +x /usr/bin/gcc-11

tee /usr/bin/g++-11 >/dev/null <<SH
#!/usr/bin/env bash
exec "$ENV/bin/x86_64-conda-linux-gnu-g++" "\$@"
SH
sudo chmod +x /usr/bin/g++-11
