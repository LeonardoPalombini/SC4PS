# A brief guide on CloudVeneto machine setup

This guide is meant to be a short reference to create a CloudVeneto virtual machine, running AlmaLinux 9.

## Prerequisites

1. Have your CloudVeneto account
2. Having requested the access to project-related resources

## Create a new VM

1. Enter your personal [CloudVeneto](https://cloudveneto.it) project dashboard
2. Create an ssh key pair (button on the left dropdown)
3. Inside the "Instances" menu, launch a new instance
4. Inside setup popup window, set all the mandatory VM options: name, origin, flavour, key pair (your own)
5. Execute the instance

## VM access setup

1. Download your key pair
2. Set key pair permissions: `chmod 600 key_pair.pem`
3. In your own `.ssh/config` add:
	Host <name>
    	HostName <ip>
    	User <username>
    	ProxyJump <CVuser>@gate.cloudveneto.it
    	IdentityFile /path/to/key_pair.pem
5. You can now access your machine: `ssh <name>`

## Compile and run C scripts

1. Install a compiler, eg: `sudo yum install gcc` or a more complete set of tools (gcc, make...): `sudo dnf groupinstall "Development Tools"`
2. Installing git is recommended: `sudo yum install git`
3. You can now compile your C code: `gcc example.C`
