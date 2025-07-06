#
# Project Chimera - Supervisor Mode Stub Assembly
#
# This is a minimal assembly stub for Supervisor mode entry.
# It simply enters an infinite loop.
#

.section .text
.global s_mode_stub

s_mode_stub:
    # Infinite loop
    j s_mode_stub
