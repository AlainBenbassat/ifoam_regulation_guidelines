# IFOAM Regulation Guidelines API

## Endpoint

/regulation-guidelines-api

Required parameters:

  * action=guidelines_access
  * email=SOME-EMAIL-ADDRESS

## Returns

A JSON object

```
{
  "status": "success"|"error",
  "description": "SOME DESCRIPTION",
  "is_member": 0|1,
  "is_free": 0|1,
  "email": "SOME-EMAIL-ADDRESS"
}
```