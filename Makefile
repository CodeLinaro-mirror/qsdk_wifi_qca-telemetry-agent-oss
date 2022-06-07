obj-m += telemetry_agent.o
telemetry_agent-objs := src/telemetry_agent.o src/telemetry_agent_sawf.o

all:
	make -C $(KERNEL_PATH) M=$(PWD) modules

clean:
	make -C $(KERNEL_PATH) M=$(PWD) clean
