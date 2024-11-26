<script lang="ts">
	import { onMount } from 'svelte';
	import Separator from './shadcn-components/ui/separator/separator.svelte';
	import { type Device } from '$lib/server/db/schema';


	let map: Map | LayerGroup<any>;
	let circle;
	let markers = [];
	let radius = 1000; // Default radius in meters

	// svelte-ignore export_let_unused
	let props = $props();

	console.log("esempio:"+props.devices);

	type User_Position = {
		latitude: number;
		longitude: number;
	};

	onMount(async () => {
		// Dynamically import Leaflet to avoid SSR issues
		const L = await import('leaflet');
		import('leaflet/dist/leaflet.css');

		let user_position = await getUserPosition();

		const circle_center = [user_position.latitude, user_position.longitude];

		map = L.map('map', {
			center: [user_position.latitude ?? 100, user_position.longitude ?? 100], // Reggio Emilia, Italy coordinates
			zoom: 13, // Initial zoom level
			minZoom: 2.5, // Restricts zooming out too far
			maxBounds: [
				[-90, -180], // Southwest corner (entire world)
				[90, 180] // Northeast corner (entire world)
			],
			maxBoundsViscosity: 1.0 // Prevents panning beyond the bounds
		});

		props.devices.forEach((element: Device) => {
			L.marker([element.lastknownlocation?.x.valueOf()!,element.lastknownlocation?.y.valueOf()!])
			.addTo(map)
			.bindPopup(element.name);
		});

		L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
			maxZoom: 19,
			attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
		}).addTo(map);

		L.control.scale().addTo(map);
		L.marker([user_position.latitude ?? 100, user_position.longitude ?? 100])
			.addTo(map)
			.bindPopup('You are here!')
			.openPopup();

		circle = L.circle(circle_center, {
			color: 'green',
			fillColor: '#bfffd0',
			fillOpacity: 0.3,
			radius: radius
		}).addTo(map);
	});

	function updateRadius() {
		if (circle) {
			circle.setRadius(radius);
		}
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

<div class="flex flex-row">
	<div class="flex-grow bg-white border-2 rounded-md p-2 m-2">
		<h1 class=" font-bold">C.R.A.S.H Map Locations</h1>
		<div id="map" class="mt-1"></div>
	</div>
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

<style>
	#map {
		height: 400px;
		width: 100%;
	}
	button {
		margin-top: 10px;
		padding: 10px;
		cursor: pointer;
	}
</style>
