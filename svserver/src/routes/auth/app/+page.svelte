<script lang="ts">
	import MapComponent from '$lib/components/MapComponent.svelte';
	import type { Props } from '$lib/components/MapComponent.svelte';
	import crashlogo from '$lib/assets/383990724-d3bdb664-5fdc-446d-9361-8037283497e7.png';
	import * as Avatar from '$lib/components/shadcn-components/ui/avatar/index';
	import Separator from '$lib/components/shadcn-components/ui/separator/separator.svelte';
	import { Switch } from '$lib/components/ui/switch';
	import { Label } from '$lib/components/ui/label';
	import { Button } from '$lib/components/ui/button';
	import { Input } from '$lib/components/ui/input';
	import { goto } from '$app/navigation';
	import Skeleton from '$lib/components/ui/skeleton/skeleton.svelte';
	import { LineChart} from 'layerchart';

	const { data } = $props();
	let crashsites = $state(data.crashList);
	//let predictions = $state(data.streamed.predictions);

	let radius = $state(data.radius);
	let activedevice = $state(data.activedevice);
	let predictions_enabled = $state(data.predictions_enabled);

	let deviceid = $derived(activedevice.id);

	$effect(() => {
		crashsites = data.crashList;
		//predictions = data.streamed.predictions;

		radius = data.radius;
		activedevice = data.activedevice;
		predictions_enabled = data.predictions_enabled;
	});

	function refreshPage(p?: { r?: number; deviceid?: string }) {
		const params = new URLSearchParams({
			r: (p?.r ?? radius).toString(),
			deviceid: p?.deviceid ?? deviceid,
			predictions: predictions_enabled.toString()
		});

		return goto(`?${params}`);
	}

	/*
	async function getUserPosition() {
		return new Promise<User_Position>((resolve, reject) => {
			navigator.geolocation.getCurrentPosition((position) => {
				let user_position = {
					latitude: position.coords.latitude,
					longitude: position.coords.longitude
				};
				resolve(user_position);
			}, reject);
		});
	}
	*/

	async function ondeviceclick(device: Parameters<NonNullable<Props['ondeviceclick']>>[0]) {
		activedevice = data.deviceList.find((x) => x.id === device.id)!;
		await refreshPage();
	}
</script>

<!-- Header -->
<div class="flex flex-row items-center border-b-2 shadow-md">
	<div class="mr-4">
		<img alt="Logo di CRASH" src={crashlogo} width="50px" />
	</div>
	<div class=" flex-grow">
		<h1 class=" font-bold">C.R.A.S.H - Collision Check System</h1>
	</div>
	<div class="mx-4">
		<Avatar.Root>
			<Avatar.Image src="https://github.com/shadcn.png" alt="@shadcn" />
			<Avatar.Fallback>CN</Avatar.Fallback>
		</Avatar.Root>
	</div>
</div>

<div class="m-2 flex flex-col flex-1 gap-2">
	<div class="p-4 border-1 rounded-md bg-blue-200">
		<h2 class="font-bold text-lg">Welcome in C.R.A.S.H!</h2>
		<p>
			In the home page You can see the informations given by the devices connected at the network
			and analyze datas based on the range choosen
		</p>
	</div>

	<!-- Map + settings -->
	<div class="flex flex-col flex-1">
		<div class="grid grid-cols-3 grid-rows-1 flex-1 gap-2">
			<div class="col-span-2 flex flex-col flex-1 gap-2 bg-white border-2 rounded-md p-2">
				<h3 class=" font-bold">C.R.A.S.H Map Locations</h3>
				<MapComponent
					devices={data.deviceList}
					{ondeviceclick}
					crashSites={crashsites}
					{activedevice}
					{radius}
					class="flex-1"
				/>
			</div>

			<div class="flex flex-col gap-2 full">
			<!--div class="grid grid-rows-[1fr_1fr_1fr_3fr] gap-2 h-full"-->
				<div class="flex flex-col bg-white border-2 rounded-md p-2 gap-2">
					<h3 class="font-bold">Settings Panel</h3>
					<Separator />
					<div class="flex-1 flex flex-col gap-4">
						<div class="flex flex-col gap-2">
							<Label for="radius">Adjust Radius (Km):</Label>
							<div class="flex items-center gap-2">
								<Input
									type="range"
									id="radius"
									name="radius"
									min="1"
									max="150"
									bind:value={radius}
									onchange={() => refreshPage()}
								/>
								<Input
									type="number"
									id="nradius"
									name="nradius"
									min="1"
									max="150"
									bind:value={radius}
									onchange={() => refreshPage()}
									class="w-fit"
								/>
								<span>Km</span>
							</div>
						</div>
					</div>
				</div>
				<div class="flex flex-col bg-white border-2 rounded-md p-2 gap-2">
					<h3 class="font-bold">Generate test crashes</h3>
					<Separator />
					<div class="flex flex-col gap-2">
						<form name="gencrash" method="POST" action="?/gencrash" class="flex flex-col gap-2">
							<Input name="radius" value={radius} type="hidden" />
							<Input name="deviceid" value={deviceid} type="hidden" />

							<Label for="number">Number</Label>
							<Input name="number" type="number" max="200" min="0" class="w-1/2"/>

							<Button type="submit" variant="outline" class="self-end">Generate</Button>
						</form>
					</div>
				</div>
				<div class="bg-white border-2 rounded-md p-4">
					<h3 class=" font-bold">Devices Installed</h3>
					<ul>
						{#each data.deviceList as device}
							<li>
								<span class="px-2 font-normal italic">{device.name}</span>
								<button
									class:hidden={!device.active}
									disabled={!device.active}
									onclick={() => ondeviceclick(device)}
									>go to
								</button>
							</li>
						{/each}
					</ul>
				</div>
				<div class="bg-white border-2 rounded-md p-4 flex gap-4 flex-col flex-1">
					<div class="flex justify-between">
						<h3 class=" font-bold">C.R.A.S.H Prevision</h3>
						<Switch
							id="predictions"
							bind:checked={predictions_enabled}
							onCheckedChange={() => refreshPage()}
						/>
					</div>
					{#if predictions_enabled}
						{#await data.streamed.predictions}
							<Skeleton />
						{:then pr}
							{#if pr?.length}
							<LineChart 
								data={pr} 
								x="ds" 
								series={[{ key: "yhat", color: "#FF0000", label: 'Crashes' }]}
								/>
							{:else}
							<span class="px-2 font-normal italic">Couldn't compute any predictions</span>
							{/if}
						{/await}
					{:else}
						enable pred
					{/if}
				</div>
			</div>
		</div>

		<!-- Device list -->
	</div>
</div>
