<script lang="ts">
	import { type Circle, type LatLngExpression, type Map } from 'leaflet';
	import { onMount } from 'svelte';

	let map: Map;
	let circle: Circle<any>;

	interface Props {
		devices: { name: string; lat: number; long: number }[];
		radius: number;
		crashSites: { lat: number; long: number; deviceId: string; time: Date | null }[];
		user_position: User_Position;
	}

	let { devices, radius, crashSites, user_position }: Props = $props();

	type User_Position = {
		latitude: number;
		longitude: number;
	};

	$effect(() => {
		const r = radius * 1000;
		if (circle) {
			circle.setRadius(r);
		}
	});

	onMount(async () => {
		// Dynamically import Leaflet to avoid SSR issues
		try {
			const L = await import('leaflet');
			import('leaflet/dist/leaflet.css');

			
			const circle_center: LatLngExpression = [user_position.latitude, user_position.longitude];
			
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
			
			const myCustomColour = '#583470';

			const markerHtmlStyles = `
  background-color: ${myCustomColour};
  width: 2rem;
  height: 2rem;
  display: block;
  left: -1rem;
  top: -1rem;
  position: relative;
  border-radius: 3rem 3rem 0;
  transform: rotate(45deg);
  border: 1px solid #FFFFF`;

			const icon = L.divIcon({
				className: 'my-custom-pin',
				iconAnchor: [0, 24],
				popupAnchor: [0, -36],
				html: `<span style="${markerHtmlStyles}" />`
			});
			
			if (devices != undefined) {
				devices.forEach((element) => {
					L.marker([element.lat, element.long], { icon: icon })
						.addTo(map)
						.bindPopup(element.name);
				});
			}

			const myCustomColourCrash = '#aebedf';

			const markerHtmlStylesCrash = `
  background-color: ${myCustomColourCrash};
  width: 2rem;
  height: 2rem;
  display: block;
  left: -1rem;
  top: -1rem;
  position: relative;
  border-radius: 2rem 2rem 0;
  transform: rotate(45deg);
  border: 1px solid #FFFFFF`;

			const iconCrash = L.divIcon({
				className: 'my-custom-pin',
				iconAnchor: [0, 24],
				popupAnchor: [0, -36],
				html: `<span style="${markerHtmlStylesCrash}" />`
			});

			if (crashSites != undefined) {
				crashSites.forEach((element) => {
					L.marker([element.lat, element.long], { icon: iconCrash })
						.addTo(map)
						.bindPopup(element.deviceId + ' - ' + element.time);
				});
			}
			
			L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
				maxZoom: 19,
				attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
			}).addTo(map);
			
			L.control.scale().addTo(map);

			const myCustomColourUser = 'rgb(0,255,0)';

			const markerHtmlStylesUser = `
  background-color: ${myCustomColourUser};
  width: 2rem;
  height: 2rem;
  display: block;
  left: -1rem;
  top: -1rem;
  position: relative;
  border-radius: 3rem 3rem 0;
  transform: rotate(45deg);
  border: 1px solid #FFFFFF`;

			const iconUser = L.divIcon({
				className: 'my-custom-pin',
				iconAnchor: [0, 24],
				popupAnchor: [0, -36],
				html: `<span style="${markerHtmlStylesUser}" />`
			});
			L.marker([user_position.latitude ?? 100, user_position.longitude ?? 100],{icon:iconUser})
				.addTo(map)
				.bindPopup('You are here!')
				.openPopup();

			circle = L.circle(circle_center, {
				color: 'green',
				fillColor: '#bfffd0',
				fillOpacity: 0.3,
				radius: radius * 1000
			}).addTo(map);
		} catch (error) {
			console.log(error);
		}
	});
</script>

<div class="flex-grow bg-white border-2 rounded-md p-2 m-2">
	<h1 class=" font-bold">C.R.A.S.H Map Locations</h1>
	<div id="map" class="mt-1"></div>
</div>

<style>
	#map {
		height: 400px;
		width: 100%;
	}
</style>
