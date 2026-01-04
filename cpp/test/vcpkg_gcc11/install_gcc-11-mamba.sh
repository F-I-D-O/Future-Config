#!/usr/bin/env bash
set -euo pipefail

MAMBA_ENV_PARENT="$HOME/.mamba"

## Install mamba
#sudo dnf install mamba -y
#sudo tee /etc/profile.d/mamba.sh >/dev/null <<'EOF'
## Configure micromamba / mamba root prefix for non-root users
#export MAMBA_ROOT_PREFIX=MAMBA_ENV_PARENT
#export MICROMAMBA_ROOT_PREFIX=MAMBA_ENV_PARENT
#EOF
#source /etc/profile.d/mamba.sh
#
#mamba create -n gcc11 -c conda-forge gcc_linux-64=11 gxx_linux-64=11 -y

# Create wrapper scripts for gcc-11 and g++-11
GCC_ENV_PATH="$MAMBA_ENV_PARENT/envs/gcc11"
sudo tee /usr/local/bin/gcc-11 >/dev/null <<SH
#!/usr/bin/env bash
exec "$GCC_ENV_PATH/bin/x86_64-conda-linux-gnu-gcc" "\$@"
SH
sudo chmod +x /usr/local/bin/gcc-11

sudo tee /usr/local/bin/g++-11 >/dev/null <<SH
#!/usr/bin/env bash
exec "$GCC_ENV_PATH/bin/x86_64-conda-linux-gnu-g++" "\$@"
SH
sudo chmod +x /usr/local/bin/g++-11
