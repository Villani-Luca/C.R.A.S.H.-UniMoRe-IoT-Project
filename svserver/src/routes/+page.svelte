<script lang="ts">
	import MapComponent from '$lib/components/MapComponent.svelte';
	import crashlogo from '$lib/assets/383990724-d3bdb664-5fdc-446d-9361-8037283497e7.png';
	import * as Avatar from '$lib/components/shadcn-components/ui/avatar/index';
	import type { PageData } from './$types';
	import Separator from '$lib/components/shadcn-components/ui/separator/separator.svelte';

	export let data: PageData;
    let radius: number = 1000;
</script>

<div class="flex flex-row items-center border-b-2 shadow-md">
	<div class="mr-4">
		<img src={crashlogo} width="50px" />
	</div>
	<div class=" flex-grow">
		<h1 class=" font-bold">C.R.A.S.H - Collision Check System</h1>
	</div>
	<div class="">
		<h1 class=" font-bold">Welcome, Luca Villani</h1>
	</div>
	<div class="mx-4">
		<Avatar.Root>
			<Avatar.Image src="https://github.com/shadcn.png" alt="@shadcn" />
			<Avatar.Fallback>CN</Avatar.Fallback>
		</Avatar.Root>
	</div>
</div>

<div class="p-4 border-1 rounded-md bg-blue-200 m-2">
	<h1 class=" font-bold text-lg">Welcome in C.R.A.S.H!</h1>
	<p>
		In the home page You can see the informations given by the devices connected at the network and
		analyze datas based on the range choosen
	</p>
</div>

<div class="flex flex-row">
	<MapComponent devices={data.deviceList} radius={radius} crashSites={[]}/>
	<div class="bg-white border-2 rounded-md p-4 m-2">
		<div class="settings-panel">
			<div>
				<p class=" font-bold">Settings Panel</p>
			</div>
			<Separator class="my-1" />
			<label for="radius">Adjust Radius (meters):</label>
			<input
				type="range"
				id="radius"
				min="100"
				max="100000"
				bind:value={radius}
				on:input={updateRadius()}
			/>
			<label> {radius}</label>
		</div>
	</div>
</div>

<div class="grid grid-cols-2">
	<div class="bg-white border-2 rounded-md p-4 m-2">
		<h1 class=" font-bold">Devices Installed</h1>
		<ul>
			{#each data.deviceList as device}
				<li>
					<label class="px-2 font-normal italic">{device.name}</label>
				</li>
			{/each}
		</ul>
	</div>
	<div class="bg-white border-2 rounded-md p-4 m-2">
		<h1 class=" font-bold">C.R.A.S.H Prevision</h1>
		<p class="font-light italic text-sm">Previsions of the crashes in the range choose</p>
	</div>
</div>
