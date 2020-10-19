#!/bin/bash

runcpu --config=RISC-V-dbt --size=ref --noreportable intspeed;
runcpu --config=RISC-V-dbt_no-fusion --size=ref --noreportable intspeed;
runcpu --config=RISC-V-dbt_no-ras --size=ref --noreportable intspeed;
runcpu --config=RISC-V-dbt_no-jump-no-ras --size=ref --noreportable intspeed;
runcpu --config=RISC-V-dbt_none --size=ref --noreportable 623;
