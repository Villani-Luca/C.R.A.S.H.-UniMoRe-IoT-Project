<script lang="ts">
	import MapComponent from '$lib/components/MapComponent.svelte';
	import crashlogo from '$lib/assets/383990724-d3bdb664-5fdc-446d-9361-8037283497e7.png';
	import * as Avatar from '$lib/components/shadcn-components/ui/avatar/index';
	import Separator from '$lib/components/shadcn-components/ui/separator/separator.svelte';
	import { Skeleton } from '$lib/components/ui/skeleton';
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
		console.log(params);
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

<!-- Header -->
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
			{#await user_position}
				<Skeleton class="col-span-2 border-2 rounded-md p-2" />
				<div class="grid grid-rows-5 gap-2">
					<Skeleton class="row-span-2 border-2 rounded-md p-2" />
					<Skeleton class="row-span-2 border-2 rounded-md p-2" />
					<Skeleton class="border-2 rounded-md p-2" />
				</div>
			{:then up}
				<div class="col-span-2 flex flex-col flex-1 gap-2 bg-white border-2 rounded-md p-2">
					<h3 class=" font-bold">C.R.A.S.H Map Locations</h3>
					<MapComponent
						devices={data.deviceList}
						{radius}
						crashSites={data.crashList}
						userposition={up}
						class="flex-1"
					/>
				</div>

				<div class="grid grid-rows-5 gap-2">
					<div class="flex flex-col bg-white border-2 rounded-md p-2 row-span-2 gap-2">
						<h3 class="font-bold">Settings Panel</h3>
						<Separator />
						<div class="flex-1 flex flex-col justify-between">
							<label for="radius">
								Adjust Radius (Km):
								<div class="flex items-center gap-2">
									<input
										type="range"
										id="radius"
										name="radius"
										min="1"
										max="150"
										bind:value={radius}
										onchange={() => goToPage(radius, up.latitude, up.longitude)}
									/>
									<input
										type="number"
										id="nradius"
										name="nradius"
										min="1"
										max="150"
										bind:value={radius}
										onchange={() => goToPage(radius, up.latitude, up.longitude)}
									/>
									<span>Km</span>
								</div>
							</label>
						</div>
					</div>
					<div class="bg-white border-2 rounded-md p-4 row-span-2">
						<h3 class=" font-bold">Devices Installed</h3>
						<ul>
							{#each data.deviceList as device}
								<li>
									<span class="px-2 font-normal italic">{device.name}</span>
								</li>
							{/each}
						</ul>
					</div>
					<div class="bg-white border-2 rounded-md p-4">
						<h3 class=" font-bold">C.R.A.S.H Prevision</h3>
						<p class="font-light italic text-sm">Previsions of the crashes in the range choose</p>
						<button onclick={() => goto(`/auth/app/prediction`)}>Go to AI predictions...</button>
					</div>
				</div>
			{:catch error}
				<p style="color: red">{error.message}</p>
			{/await}
		</div>

		<!-- Device list -->
	</div>
</div>
