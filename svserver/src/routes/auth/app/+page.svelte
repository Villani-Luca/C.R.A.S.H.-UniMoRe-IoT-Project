<script lang="ts">
	import MapComponent from '$lib/components/MapComponent.svelte';
	import crashlogo from '$lib/assets/383990724-d3bdb664-5fdc-446d-9361-8037283497e7.png';
	import * as Avatar from '$lib/components/shadcn-components/ui/avatar/index';
	import Separator from '$lib/components/shadcn-components/ui/separator/separator.svelte';
	import { type User_Position } from '$lib/helpers/user_position';
	import { goto } from '$app/navigation';
	

	const { data } = $props();

	let radius = $state(data.radius);
	let user_position = getUserPosition();

	let searchForm: HTMLFormElement;

	function goToPage(radius: number, lat: number, long: number): null {
		const params = new URLSearchParams({
			r: radius.toString(),
			lat: lat.toString(),
			long: long.toString()
		});
		goto(`?${params}`);
		return null;
	}

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
</script>

<div class="flex flex-row items-center border-b-2 shadow-md">
	<div class="mr-4">
		<img alt="Logo di CRASH" src={crashlogo} width="50px" />
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

<div class="grid grid-cols-3">
	{#await user_position}
		<p>Getting user position...</p>
	{:then up}
		<div class=" col-span-2">
			<MapComponent
				devices={data.deviceList}
				radius= {radius}
				crashSites={data.crashList}
				user_position={up}
			/>
		</div>
		<div class="bg-white border-2 rounded-md p-4 m-2">
			<div class="settings-panel">
				<div>
					<p class=" font-bold">Settings Panel</p>
				</div>
				<Separator class="my-1" />
				<label for="radius">Adjust Radius (Km):</label>
					<input
						type="range"
						id="radius"
						name="radius"
						min="1"
						max="150"
						bind:value={radius}
					/>
					<span> {radius} Km</span>
					<button onclick={goToPage(radius, up.latitude, up.longitude)}> Cerca </button>
			</div>
		</div>
	{:catch error}
		<p style="color: red">{error.message}</p>
	{/await}
</div>

<div class="grid grid-cols-2">
	<div class="bg-white border-2 rounded-md p-4 m-2">
		<h1 class=" font-bold">Devices Installed</h1>
		<ul>
			{#each data.deviceList as device}
				<li>
					<span class="px-2 font-normal italic">{device.name}</span>
				</li>
			{/each}
		</ul>
	</div>
	<div class="bg-white border-2 rounded-md p-4 m-2">
		<h1 class=" font-bold">C.R.A.S.H Prevision</h1>
		<p class="font-light italic text-sm">Previsions of the crashes in the range choose</p>
	</div>
</div>
